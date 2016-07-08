#include "chig/GraphFunction.hpp"
#include "chig/NodeType.hpp"

#include <llvm/Support/raw_ostream.h>
#include <llvm/AsmParser/Parser.h>
#include <llvm/Support/SourceMgr.h>


using namespace chig;

GraphFunction::GraphFunction(Context& context, std::string name, const std::vector<std::pair<llvm::Type*, std::string>>& inputs, const std::vector<std::pair<llvm::Type*, std::string>>& argOutputs)
	: graphName{std::move(name)},
	outputs{argOutputs},
	owningContext{&context} {
	
	nodes.emplace_back(std::make_unique<NodeInstance>(std::make_unique<EntryNodeType>(inputs), 0.f, 0.f));
	entry = nodes[0].get();
	
}

GraphFunction GraphFunction::fromJSON(Context& context, const nlohmann::json& data) {
	
	if(data["type"] != "function") {
		throw std::runtime_error("Error deserializing GraphFunction: JSON data wasn't a function");
	}
	std::string name = data["name"];
	
	// get inputs
	// TODO: user-made types
	
	// TODO: less hacky way to get types
	// This generates some IR with that type then extracts that. Yeah it's hacky
	std::vector<std::pair<llvm::Type*, std::string>> inputs;
	for(auto inIter = data["inputs"].begin(); inIter != data["inputs"].end(); ++inIter) {
		std::string typeStr = inIter.value();
		auto IR = "@G = external global " + typeStr;
		auto err = llvm::SMDiagnostic();
		auto tmpModule = llvm::parseAssemblyString(IR, err, context.context);
		inputs.push_back({tmpModule->getNamedValue("G")->getType(), inIter.key()});
	}
	
	// same for outptus
	std::vector<std::pair<llvm::Type*, std::string>> outputs;
	for(auto outIter = data["outptus"].begin(); outIter != data["outputs"].end(); ++outIter) {
		std::string typeStr = outIter.value();
		auto IR = "@G = external global " + typeStr;
		auto err = llvm::SMDiagnostic();
		auto tmpModule = llvm::parseAssemblyString(IR, err, context.context);
		outputs.push_back({tmpModule->getNamedValue("G")->getType(), outIter.key()});
	}
	
	// construct it
	GraphFunction ret(context, name, inputs, outputs);
	
	// read the nodes
	for(const auto& node : data["nodes"]) {
		std::string fullType = node["type"];
		std::string moduleName = fullType.substr(0, fullType.find(':'));
		std::string typeName = fullType.substr(fullType.find(':') + 1);
		
		auto* moduleWithNodeType = context.getModuleByName(moduleName.c_str());
		
		auto nodeType = moduleWithNodeType->createNodeType(typeName.c_str(), node["data"]);
		
		ret.nodes.push_back(std::make_unique<NodeInstance>(std::move(nodeType), node["location"][0], node["location"][0]));
		
	}
	
	// read the connections
	
	
}

nlohmann::json GraphFunction::toJSON() {
	auto jsonData = nlohmann::json{};
	
	jsonData["type"] = "function";
	jsonData["name"] = graphName;
	
	// create inputs JSON
	auto& inputsJson = jsonData["inputs"];
	inputsJson = nlohmann::json::object();
	for(size_t i = 0; i < entry->type->outputs.size(); ++i) {
		std::string type;

		llvm::raw_string_ostream stream{type};
		entry->type->outputs[i].first->print(stream);
		stream.flush();
		
		inputsJson[entry->type->outputs[i].second] = type;
	}
	
	// create outputs JSON
	auto& outputJson = jsonData["outputs"];
	outputJson = nlohmann::json::object();
	for(size_t i = 0; i < outputs.size(); ++i) {
		std::string type;
		llvm::raw_string_ostream stream{type};
		outputs[i].first->print(stream);
		stream.flush();
		
		outputJson[outputs[i].second] = type;
	}
	
	// serialize the nodes
	auto& jsonNodes = jsonData["nodes"];
	auto& jsonConnections = jsonData["connections"];
	
	for(auto node_id = 0ull; node_id < nodes.size(); ++node_id) {
		auto& node = nodes[node_id];
		jsonNodes.push_back({
			{"type", node->type->module + ':' + node->type->name},
			{"location", {node->x, node->y}}
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

llvm::Function* GraphFunction::compile() {
}

NodeInstance* GraphFunction::insertNode(std::unique_ptr<NodeType> type, float x, float y) {
	auto ptr = std::make_unique<NodeInstance>(std::move(type), x, y);
	
	nodes.push_back(std::move(ptr));
	
	return nodes[nodes.size() - 1].get();
}

