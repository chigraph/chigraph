#include "chig/GraphFunction.hpp"
#include "chig/NodeType.hpp"

#include <llvm/AsmParser/Parser.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/raw_ostream.h>
#include <unordered_map>

using namespace chig;

GraphFunction::GraphFunction(Context& context, std::string name)
	: graphName{std::move(name)}, owningContext{&context}
{
}

GraphFunction GraphFunction::fromJSON(Context& context, const nlohmann::json& data)
{
	if (data["type"] != "function") {
		throw std::runtime_error("Error deserializing GraphFunction: JSON data wasn't a function");
	}
	std::string name = data["name"];

	// construct it
	GraphFunction ret(context, name);

	// read the nodes
	if (data.find("nodes") == data.end()) {
		throw std::runtime_error(
			"Error reading JOSN: each function needs an nodes JSON array of objects. JSON dump: " +
			data.dump());
	}
	for (const auto& node : data["nodes"]) {
		if (node.find("type") == node.end()) {
			throw std::runtime_error(
				"Error reading JOSN: each node needs a \"type\" element. JSON dump: " +
				node.dump());
		}
		std::string fullType = node["type"];
		std::string moduleName = fullType.substr(0, fullType.find(':'));
		std::string typeName = fullType.substr(fullType.find(':') + 1);

		if (node.find("data") == node.end()) {
			throw std::runtime_error(
				"Error reading JOSN: each node needs a \"data\" object. JSON dump: " + node.dump());
		}

		auto nodeType = context.getNodeType(moduleName.c_str(), typeName.c_str(), node["data"]);
		if (!nodeType) {
			throw std::runtime_error("Error creating node type. Module \"" + moduleName +
									 "\"; Node type name: \"" + typeName + "\"");
		}
		auto testIter = node.find("location");
		if (testIter != node.end()) {
			// make sure it is the right size
			if (!testIter.value().is_array() || testIter.value().size() == 2) {
				//	throw std::runtime_error("Error reading json: node[x].location must be an array
				//of size 2 of floats. JOSN dump: " + node.dump());
			}
		} else {
			throw std::runtime_error(
				"each node much have a array of size 2 for the location. JSON dump: " +
				node.dump());
		}
		ret.nodes.push_back(std::make_unique<NodeInstance>(
			std::move(nodeType), node["location"][0], node["location"][0]));
	}

	// read the connections
	for (auto& connection : data["connections"]) {
		std::string type = connection["type"];
		bool isData = type == "data";
		// it either has to be "data" or "exec"
		if (!isData && type != "exec") {
			throw std::runtime_error("Unrecoginized data type: " + type);
		}

		int InputNodeID = connection["input"][0];
		int InputConnectionID = connection["input"][1];

		int OutputNodeID = connection["output"][0];
		int OutputConnectionID = connection["output"][1];

		// make sure the nodes exist
		if (InputNodeID >= ret.nodes.size()) {
			throw std::runtime_error("Out of bounds in input node in connection " +
									 connection.dump() + " : no node with ID " +
									 std::to_string(InputNodeID));
		}
		if (OutputNodeID >= ret.nodes.size()) {
			throw std::runtime_error("Out of bounds in output node in connection " +
									 connection.dump() + " : no node with ID " +
									 std::to_string(InputNodeID));
		}

		if (OutputNodeID == InputNodeID) {
			throw std::runtime_error(
				"Cannot connect a node to itsself! JSON dump: " + connection.dump());
		}

		// connect
		// these functions do bounds checking, it's okay
		if (isData) {
			connectData(*ret.nodes[InputNodeID], InputConnectionID, *ret.nodes[OutputNodeID],
				OutputConnectionID);
		} else {
			connectExec(*ret.nodes[InputNodeID], InputConnectionID, *ret.nodes[OutputNodeID],
				OutputConnectionID);
		}
	}

	return ret;
}

nlohmann::json GraphFunction::toJSON()
{
	auto jsonData = nlohmann::json{};

	jsonData["type"] = "function";
	jsonData["name"] = graphName;

	auto* entry = getEntryNode().first;
	if(!entry) {
		throw std::runtime_error("Not exactly one entry node in GraphFunction!");
	}
	
	// serialize the nodes
	auto& jsonNodes = jsonData["nodes"];
	auto& jsonConnections = jsonData["connections"];

	for (auto node_id = 0ull; node_id < nodes.size(); ++node_id) {
		auto& node = nodes[node_id];
		jsonNodes.push_back({{"type", node->type->module + ':' + node->type->name},
			{"location", {node->x, node->y}}, {"data", node->type->toJSON()}});
		// add its connections. Just out the outputs to avoid duplicates

		// add the exec outputs
		for (auto conn_id = 0ull; conn_id < node->outputExecConnections.size(); ++conn_id) {
			auto& conn = node->outputExecConnections[conn_id];
			// if there is actually a connection
			if (conn.first) {
				// find the ID of the second
				// TODO: there is a more effiecent way to codegen this. Cache incomplete connections
				// for both sides, then check if this node completes any of the connections
				auto other_id = std::distance(
					nodes.begin(), std::find_if(nodes.begin(), nodes.end(),
									   [&](auto& un_ptr) { return un_ptr.get() == conn.first; }));

				jsonConnections.push_back({{"type", "exec"}, {"input", {node_id, conn_id}},
					{"output", {other_id, conn.second}}});
			}
		}

		// add the data outputs
		for (auto conn_id = 0ull; conn_id < node->outputDataConnections.size(); ++conn_id) {
			auto& conn = node->outputDataConnections[conn_id];
			// if there is actually a connection
			if (conn.first) {
				// find the ID of the second
				// TODO: there is a more effiecent way to codegen this. Cache incomplete connections
				// for both sides, then check if this node completes any of the connections
				auto other_id = std::distance(
					nodes.begin(), std::find_if(nodes.begin(), nodes.end(),
									   [&](auto& un_ptr) { return un_ptr.get() == conn.first; }));

				jsonConnections.push_back({{"type", "data"}, {"input", {node_id, conn_id}},
					{"output", {other_id, conn.second}}});
			}
		}
	}

	return jsonData;
}

	
struct cache {
	std::vector<llvm::Value*> outputs;
};

// Codegens a single input to a node
void codegenHelper(NodeInstance* node, unsigned execInputID, llvm::BasicBlock* block, llvm::Function* f, std::unordered_map<NodeInstance*, cache>& nodeCache) {
	
	// get input values
	std::vector<llvm::Value*> inputParams;
	for(auto& param : node->inputDataConnections) {
		// TODO: error handling
		inputParams.push_back(nodeCache[param.first].outputs[param.second]);
	}
	
	
	auto entryBlock = &f->getEntryBlock();
	llvm::IRBuilder<> entryBuilder(entryBlock);
	
	// create outputs
	auto& outputCache = nodeCache[node].outputs;
	for(auto& output : node->type->dataOutputs) {
		// TODO: research address spaces
		llvm::AllocaInst* alloc = entryBuilder.CreateAlloca(llvm::PointerType::get(output.first, 0), nullptr); // TODO: name
		outputCache.push_back(alloc);
	}
	
	// combine 
	std::copy(outputCache.begin(), outputCache.end(), std::back_inserter(inputParams));
	
	
	// create output blocks
	std::vector<llvm::BasicBlock*> outputBlocks;
	for(auto idx = 0ull; idx < node->outputExecConnections.size(); ++idx) {
		outputBlocks.push_back(llvm::BasicBlock::Create(f->getContext(), node->type->execOutputs[idx], f));
	}
	
	// codegen
	node->type->codegen(execInputID, inputParams, block, outputBlocks);
	
	// codegen for all outputs
	for(auto idx = 0ull; idx < node->outputExecConnections.size(); ++idx) {
		auto& output = node->outputExecConnections[idx];
		if(output.first) codegenHelper(output.first, output.second, outputBlocks[idx], f, nodeCache);
	}
	

}

llvm::Function* GraphFunction::compile (llvm::Module* mod) {
	
	const auto& argument_connections = getEntryNode().first->type->dataOutputs; // ouptuts from entry are arguments
	
	// TODO: return types
	std::vector<llvm::Type*> arguments;
	arguments.reserve(argument_connections.size());
	std::transform(argument_connections.begin(), argument_connections.end(), std::back_inserter(arguments), [](const std::pair<llvm::Type*, std::string>& p) {
		return p.first;
	});
	
	llvm::Function* f = llvm::Function::Create(
		llvm::FunctionType::get(llvm::Type::getVoidTy(mod->getContext()), arguments, false), llvm::GlobalValue::LinkageTypes::ExternalLinkage, graphName, mod);
	llvm::BasicBlock* block = llvm::BasicBlock::Create(mod->getContext(), graphName, f);
	
	// follow linked list
	NodeInstance* node = getEntryNode().first;
	unsigned execInputID = node->outputExecConnections[0].second; // the exec connection to codegen from

	std::unordered_map<NodeInstance*, cache> nodeCache;
	
	// do entry, it's special
	auto& outputs = nodeCache[node].outputs;
	auto idx = 0ull;
	for(auto& arg : f->getArgumentList()) {
		arg.setName(node->type->dataOutputs[idx].second);
		outputs.push_back(&arg);
		++idx;
	}
	
	codegenHelper(node, execInputID, block, f, nodeCache);

	
}

std::pair<NodeInstance*, size_t> GraphFunction::getEntryNode() noexcept {
	auto entryFinder = [](auto& node) {
		return node->type->module == "lang" && node->type->name == "entry";
	};
	
	// find the entry
	auto entryIter = std::find_if(nodes.begin(), nodes.end(), entryFinder);
	if(entryIter == nodes.end()) {
		return {nullptr, ~0ull};
	}
	// make sure there is only 1 input
	if(std::find_if(entryIter + 1, nodes.end(), entryFinder) != nodes.end()) {
		return {nullptr, ~0ull};
	}
	
	return {entryIter->get(), std::distance(nodes.begin(), entryIter)};
	
}

NodeInstance* GraphFunction::insertNode(std::unique_ptr<NodeType> type, float x, float y)
{
	auto ptr = std::make_unique<NodeInstance>(std::move(type), x, y);

	nodes.push_back(std::move(ptr));

	return nodes[nodes.size() - 1].get();

}

