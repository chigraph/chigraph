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

nlohmann::json GraphFunction::toJSON() const
{
	auto jsonData = nlohmann::json{};

	jsonData["type"] = "function";
	jsonData["name"] = graphName;
	
	// serialize the nodes
	auto& jsonNodes = jsonData["nodes"];
	jsonNodes = nlohmann::json::array(); // make sure even if it's empty it's an aray
	auto& jsonConnections = jsonData["connections"];
	jsonConnections = nlohmann::json::array(); // make sure even if it's empty it's an aray

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
	
	llvm::IRBuilder<> builder(block);
	// get input values
	std::vector<llvm::Value*> inputParams;
	for(auto& param : node->inputDataConnections) {
		// TODO: error handling
		
		// get pointers to the objects
		auto value = nodeCache[param.first].outputs[param.second];
		inputParams.push_back(value); // TODO: pass ptr to value
	}
	
	
	auto entryBlock = &f->getEntryBlock();
	llvm::IRBuilder<> entryBuilder(entryBlock);
	
	// create outputs
	auto& outputCache = nodeCache[node].outputs;
	for(auto& output : node->type->dataOutputs) {
		// TODO: research address spaces
		llvm::AllocaInst* alloc = entryBuilder.CreateAlloca(output.first, nullptr, output.second); // TODO: name
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
	node->type->codegen(execInputID, f, inputParams, block, outputBlocks);
	
	// codegen for all outputs
	for(auto idx = 0ull; idx < node->outputExecConnections.size(); ++idx) {
		auto& output = node->outputExecConnections[idx];
		if(output.first) codegenHelper(output.first, output.second, outputBlocks[idx], f, nodeCache);
	}
	

}

llvm::Function* GraphFunction::compile (llvm::Module* mod) const {
	
	const auto& argument_connections = getEntryNode().first->type->dataOutputs; // ouptuts from entry are arguments
	
	// get return types;
	auto ret = getReturnTypes();
	if(!ret) {
		throw std::runtime_error("Failed to compile function: no return type");
	}
	
	std::vector<llvm::Type*> arguments;
	arguments.reserve(argument_connections.size());
	std::transform(argument_connections.begin(), argument_connections.end(), std::back_inserter(arguments), [](const std::pair<llvm::Type*, std::string>& p) {
		return p.first;
	});
	
	// make these pointers
	std::transform(ret->begin(), ret->end(), std::back_inserter(arguments), [](llvm::Type* t) {
		return llvm::PointerType::get(t, 0);
	});
	
	llvm::Function* f = llvm::Function::Create(
		llvm::FunctionType::get(llvm::IntegerType::getInt32Ty(mod->getContext()), arguments, false), llvm::GlobalValue::LinkageTypes::ExternalLinkage, graphName, mod);
	llvm::BasicBlock* block = llvm::BasicBlock::Create(mod->getContext(), graphName, f);
	
	// follow linked list
	NodeInstance* node = getEntryNode().first;
	unsigned execInputID = node->outputExecConnections[0].second; // the exec connection to codegen from

	std::unordered_map<NodeInstance*, cache> nodeCache;
	
	auto outputNode = getNodesWithType("lang", "exit")[0].first;
	
	// do entry, it's special
	auto& outputs = nodeCache[node].outputs;
	auto idx = 0ull;
	for(auto& arg : f->getArgumentList()) {
		if(idx < node->type->dataOutputs.size()) {
			arg.setName(node->type->dataOutputs[idx].second);
		} else {
			arg.setName(outputNode->type->dataInputs[idx - node->type->dataOutputs.size()].second);
		}
		outputs.push_back(&arg);
		++idx;
	}
	
	codegenHelper(node, execInputID, block, f, nodeCache);
	
	return f;
}

std::pair<NodeInstance*, size_t> GraphFunction::getEntryNode() const noexcept {

	auto matching = getNodesWithType("lang", "entry");
	
	if(matching.size() == 1) {
		return matching[0];
	}
	return {nullptr, ~0};
}

std::vector<std::pair<NodeInstance *, size_t> > GraphFunction::getNodesWithType(const char* module, const char* name) const noexcept
{
	auto typeFinder = [&](auto& node) {
		return node->type->module == module && node->type->name == name;
	};
	
	std::vector<std::pair<NodeInstance*, size_t>> ret;
	auto iter = std::find_if(nodes.begin(), nodes.end(), typeFinder);
	while(iter != nodes.end()) {
		ret.emplace_back(iter->get(), std::distance(nodes.begin(), iter));
		
		iter = std::find_if(iter + 1, nodes.end(), typeFinder);
	}
	
	return ret;
}

boost::optional<std::vector<llvm::Type*>> GraphFunction::getReturnTypes() const noexcept
{
	auto matching = getNodesWithType("lang", "exit");
	
	if(matching.size() == 0) return {};
	
	auto& types = matching[0].first->type->dataInputs;
	
	// make sure they are all the same
	if(!std::all_of(matching.begin(), matching.end(), [&](auto pair) {
		return std::equal(types.begin(), types.end(), pair.first->type->dataInputs.begin());
	})) {
		return {};
	}
	
	std::vector<llvm::Type*> ret;
	ret.resize(types.size());
	std::transform(types.begin(), types.end(), ret.begin(), [](auto pair){return pair.first;});
	
	return ret;
}


NodeInstance* GraphFunction::insertNode(std::unique_ptr<NodeType> type, float x, float y)
{
	auto ptr = std::make_unique<NodeInstance>(std::move(type), x, y);

	nodes.push_back(std::move(ptr));

	return nodes[nodes.size() - 1].get();

}

