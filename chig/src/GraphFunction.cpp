#include "chig/GraphFunction.hpp"
#include "chig/NodeType.hpp"

#include <llvm/Support/raw_ostream.h>
#include <llvm/AsmParser/Parser.h>
#include <llvm/Support/SourceMgr.h>


using namespace chig;

GraphFunction::GraphFunction(Context& context, std::string name)
	: graphName{std::move(name)},
	owningContext{&context} {
	
}

GraphFunction GraphFunction::fromJSON(Context& context, const nlohmann::json& data) {
	
	if(data["type"] != "function") {
		throw std::runtime_error("Error deserializing GraphFunction: JSON data wasn't a function");
	}
	std::string name = data["name"];
	
	// construct it
	GraphFunction ret(context, name);
	
	// read the nodes
	if(data.find("nodes") == data.end()) {
		throw std::runtime_error("Error reading JOSN: each function needs an nodes JSON array of objects. JSON dump: " + data.dump());
	}
	for(const auto& node : data["nodes"]) {
		if(node.find("type") == node.end()) {
			throw std::runtime_error("Error reading JOSN: each node needs a \"type\" element. JSON dump: " + node.dump());
		}
		std::string fullType = node["type"];
		std::string moduleName = fullType.substr(0, fullType.find(':'));
		std::string typeName = fullType.substr(fullType.find(':') + 1);
		
		if(node.find("data") == node.end()) {
			throw std::runtime_error("Error reading JOSN: each node needs a \"data\" object. JSON dump: " + node.dump());
		}
		
		auto nodeType = context.getNodeType(moduleName.c_str(), typeName.c_str(), node["data"]);
		if(!nodeType) {
			throw std::runtime_error("Error creating node type. Module \"" + moduleName + "\"; Node type name: \"" + typeName + "\"");
		}
		auto testIter = node.find("location");
		if(testIter != node.end()) {
			// make sure it is the right size
			if(!testIter.value().is_array() || testIter.value().size() == 2) {
			//	throw std::runtime_error("Error reading json: node[x].location must be an array of size 2 of floats. JOSN dump: " + node.dump());
			}
		} else {
			throw std::runtime_error("each node much have a array of size 2 for the location. JSON dump: " + node.dump());
		}
		ret.nodes.push_back(std::make_unique<NodeInstance>(std::move(nodeType), node["location"][0], node["location"][0]));
		
	}
	
	// read the connections
	for(auto& connection : data["connections"]) {
		
		std::string type = connection["type"];
		bool isData = type == "data";
		// it either has to be "data" or "exec"
		if(!isData && type != "exec") {
			throw std::runtime_error("Unrecoginized data type: " + type);
		}
		
		int InputNodeID = connection["input"][0];
		int InputConnectionID = connection["input"][1];
		
		int OutputNodeID = connection["output"][0];
		int OutputConnectionID = connection["output"][1];
		
		// make sure the nodes exist
		if(InputNodeID >= ret.nodes.size()) {
			throw std::runtime_error("Out of bounds in input node in connection " + connection.dump() + " : no node with ID " + std::to_string(InputNodeID));
		}
		if(OutputNodeID >= ret.nodes.size()) {
			throw std::runtime_error("Out of bounds in output node in connection " + connection.dump() + " : no node with ID " + std::to_string(InputNodeID));
		}
		
		if(OutputNodeID == InputNodeID) {
			throw std::runtime_error("Cannot connect a node to itsself! JSON dump: " + connection.dump());
		}
		
		// connect
		// these functions do bounds checking, it's okay
		if(isData) {
			connectData(*ret.nodes[InputNodeID], InputConnectionID, *ret.nodes[OutputNodeID], OutputConnectionID);
		} else {
			connectExec(*ret.nodes[InputNodeID], InputConnectionID, *ret.nodes[OutputNodeID], OutputConnectionID);
		}
	}
	
	return ret;
}

nlohmann::json GraphFunction::toJSON() {
	auto jsonData = nlohmann::json{};
	
	jsonData["type"] = "function";
	jsonData["name"] = graphName;
	
	auto entryFinder = [](auto& node) {
		return node->type->module == "lang" && node->type->name == "entry";
	};
	
	// find the entry
	auto entryIter = std::find_if(nodes.begin(), nodes.end(), entryFinder);
	if(entryIter == nodes.end()) {
		throw std::runtime_error("Error: no input node");
	}
	// make sure there is only 1 input
	if(std::find_if(entryIter + 1, nodes.end(), entryFinder) != nodes.end()) {
		throw std::runtime_error("Error: you cannot have two input nodes!");
	}
	
	auto* entry = entryIter->get();
	
	// serialize the nodes
	auto& jsonNodes = jsonData["nodes"];
	auto& jsonConnections = jsonData["connections"];
	
	for(auto node_id = 0ull; node_id < nodes.size(); ++node_id) {
		auto& node = nodes[node_id];
		jsonNodes.push_back({
			{"type", node->type->module + ':' + node->type->name},
			{"location", {node->x, node->y}},
			{"data", node->type->toJSON()}
		});
		// add its connections. Just out the outputs to avoid duplicates
		
		// add the exec outputs
		for(auto conn_id = 0ull; conn_id < node->outputExecConnections.size(); ++conn_id) {
			auto& conn = node->outputExecConnections[conn_id];
			// if there is actually a connection
			if(conn.first) {
				
				// find the ID of the second
				// TODO: there is a more effiecent way to codegen this. Cache incomplete connections for both sides, then check if this node completes any of the connections
				auto other_id = std::distance(nodes.begin(), std::find_if(nodes.begin(), nodes.end(), [&](auto& un_ptr){return un_ptr.get() == conn.first;}));
				
				jsonConnections.push_back({
					{"type", "exec"},
					{"input", {node_id, conn_id}},
					{"output", {other_id, conn.second}}
				});
			}
		}
		
		// add the data outputs
		for(auto conn_id = 0ull; conn_id < node->outputDataConnections.size(); ++conn_id) {
			auto& conn = node->outputDataConnections[conn_id];
			// if there is actually a connection
			if(conn.first) {
				
				// find the ID of the second
				// TODO: there is a more effiecent way to codegen this. Cache incomplete connections for both sides, then check if this node completes any of the connections
				auto other_id = std::distance(nodes.begin(), std::find_if(nodes.begin(), nodes.end(), [&](auto& un_ptr){return un_ptr.get() == conn.first;}));
				
				jsonConnections.push_back({
					{"type", "data"},
					{"input", {node_id, conn_id}},
					{"output", {other_id, conn.second}}
				});
			}
		}
		
	}
	
	return jsonData;
}

// TODO: implement
llvm::Function* GraphFunction::compile() {
	return nullptr;
}

NodeInstance* GraphFunction::insertNode(std::unique_ptr<NodeType> type, float x, float y) {
	auto ptr = std::make_unique<NodeInstance>(std::move(type), x, y);
	
	nodes.push_back(std::move(ptr));
	
	return nodes[nodes.size() - 1].get();
}

