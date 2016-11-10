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

Result GraphFunction::fromJSON(
	Context& context, const nlohmann::json& data, std::unique_ptr<GraphFunction>* ret_ptr)
{
	Result res;

	if (!data.is_object()) {
		res.add_entry("E1", "Graph json isn't a JSON object", {});
		return res;
	}
	// make sure it has a type element
	if (data.find("type") == data.end()) {
		res.add_entry("E2", "JSON in graph doesn't have a \"type\" element", {});
		return res;
	}
	if (data["type"] != "function") {
		res.add_entry("E3", "JSON in graph doesn't have a function type", {});
		return res;
	}
	// make sure there is a name
	if (data.find("name") == data.end()) {
		res.add_entry("E4", "JSON in graph doesn't have a name parameter", {});
		return res;
	}
	std::string name = data["name"];

	// construct it
	*ret_ptr = std::make_unique<GraphFunction>(context, name);
	auto& ret = *ret_ptr;

	// read the nodes
	if (data.find("nodes") == data.end()) {
		res.add_entry("E5", "JSON in graph doesn't have nodes object", {});
		return res;
	}
	size_t nodeID = 0;
	for (const auto& node : data["nodes"]) {
		if (node.find("type") == node.end()) {
			res.add_entry("E6", "Node doesn't have a type pair", {{"nodeid", nodeID}});
			return res;
		}
		std::string fullType = node["type"];
		std::string moduleName = fullType.substr(0, fullType.find(':'));
		std::string typeName = fullType.substr(fullType.find(':') + 1);

		if (moduleName.size() == 0) {
			res.add_entry("E7", "Node has an empty module name", {"nodeid", nodeID});
			return res;
		}

		if (typeName.size() == 0) {
			res.add_entry("E8", "Node has an empty type name", {"nodeid", nodeID});
			return res;
		}

		if (node.find("data") == node.end()) {
			res.add_entry("E9", "Node doens't have a data section", {"nodeid", nodeID});
			return res;
		}

		std::unique_ptr<NodeType> nodeType;
		res += context.getNodeType(moduleName.c_str(), typeName.c_str(), node["data"], &nodeType);

		if (!res) {
			return res;
		}
		auto testIter = node.find("location");
		if (testIter != node.end()) {
			// make sure it is the right size
			if (!testIter.value().is_array()) {
				res.add_entry(
					"E10", "Node doesn't have a location that is an array.", {{"nodeid", nodeID}});
				continue;
			}

			if (testIter.value().size() != 2) {
				res.add_entry("E11", "Node doesn't have a location that is an array of size 2.",
					{{"nodeid", nodeID}});
				continue;
			}
		} else {
			res.add_entry("E12", "Node doesn't have a location.", {{"nodeid", nodeID}});
			continue;
		}

		ret->nodes.push_back(std::make_unique<NodeInstance>(
			std::move(nodeType), node["location"][0], node["location"][0]));

		++nodeID;
	}

	size_t connID = 0;
	// read the connections
	for (auto& connection : data["connections"]) {
		std::string type = connection["type"];
		bool isData = type == "data";
		// it either has to be "data" or "exec"
		if (!isData && type != "exec") {
			res.add_entry("E13", "Unrecognized connection type",
				{{"connectionid", connID}, {"Found Type", type}});
			continue;
		}

		int InputNodeID = connection["input"][0];
		int InputConnectionID = connection["input"][1];

		int OutputNodeID = connection["output"][0];
		int OutputConnectionID = connection["output"][1];

		// make sure the nodes exist
		if (InputNodeID >= ret->nodes.size()) {
			res.add_entry("E14", "Input node for connection doesn't exist",
				{{"connectionid", connID}, {"Requested Node", InputNodeID}});
			continue;
		}
		if (OutputNodeID >= ret->nodes.size()) {
			res.add_entry("E15", "Output node for connection doesn't exist",
				{{"connectionid", connID}, {"Requested Node", InputNodeID}});
			continue;
		}

		// connect
		// these functions do bounds checking, it's okay
		if (isData) {
			connectData(*ret->nodes[InputNodeID], InputConnectionID, *ret->nodes[OutputNodeID],
				OutputConnectionID);
		} else {
			connectExec(*ret->nodes[InputNodeID], InputConnectionID, *ret->nodes[OutputNodeID],
				OutputConnectionID);
		}

		++connID;
	}

	return res;
}

Result GraphFunction::toJSON(nlohmann::json* toFill) const
{
	Result res;

	*toFill = nlohmann::json{};
	auto& jsonData = *toFill;

	jsonData["type"] = "function";
	jsonData["name"] = graphName;

	// serialize the nodes
	auto& jsonNodes = jsonData["nodes"];
	jsonNodes = nlohmann::json::array();  // make sure even if it's empty it's an aray
	auto& jsonConnections = jsonData["connections"];
	jsonConnections = nlohmann::json::array();  // make sure even if it's empty it's an aray

	for (auto node_id = 0ull; node_id < nodes.size(); ++node_id) {
		auto& node = nodes[node_id];

		nlohmann::json nodeJson;
		res += node->type->toJSON(&nodeJson);
		jsonNodes.push_back({{"type", node->type->module + ':' + node->type->name},
			{"location", {node->x, node->y}}, {"data", nodeJson}});
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

	return res;
}

struct cache {
	std::vector<llvm::Value*> outputs;
};

// Codegens a single input to a node
void codegenHelper(NodeInstance* node, unsigned execInputID, llvm::BasicBlock* block,
	llvm::Function* f, std::unordered_map<NodeInstance*, cache>& nodeCache)
{
	llvm::IRBuilder<> builder(block);
	// get input values
	std::vector<llvm::Value*> inputParams;
	for (auto& param : node->inputDataConnections) {
		// TODO: error handling

		// get pointers to the objects
		auto value = nodeCache[param.first].outputs[param.second];
		inputParams.push_back(value);  // TODO: pass ptr to value
	}

	auto entryBlock = &f->getEntryBlock();
	llvm::IRBuilder<> entryBuilder(entryBlock);

	// create outputs
	auto& outputCache = nodeCache[node].outputs;
	for (auto& output : node->type->dataOutputs) {
		// TODO: research address spaces
		llvm::AllocaInst* alloc =
			entryBuilder.CreateAlloca(output.first, nullptr, output.second);  // TODO: name
		outputCache.push_back(alloc);
	}

	// combine
	std::copy(outputCache.begin(), outputCache.end(), std::back_inserter(inputParams));

	// create output blocks
	std::vector<llvm::BasicBlock*> outputBlocks;
	for (auto idx = 0ull; idx < node->outputExecConnections.size(); ++idx) {
		outputBlocks.push_back(
			llvm::BasicBlock::Create(f->getContext(), node->type->execOutputs[idx], f));
	}

	// codegen
	node->type->codegen(execInputID, f, inputParams, block, outputBlocks);

	// codegen for all outputs
	for (auto idx = 0ull; idx < node->outputExecConnections.size(); ++idx) {
		auto& output = node->outputExecConnections[idx];
		if (output.first)
			codegenHelper(output.first, output.second, outputBlocks[idx], f, nodeCache);
	}
}

Result GraphFunction::compile(llvm::Module* mod, llvm::Function** ret_func) const
{
	Result res;

	const auto& argument_connections =
		getEntryNode().first->type->dataOutputs;  // ouptuts from entry are arguments

	// get return types;
	auto ret = getReturnTypes();
	if (!ret) {
		res.add_entry("E34", "No return type in functin", {});
		return res;
	}

	std::vector<llvm::Type*> arguments;
	arguments.reserve(argument_connections.size());
	std::transform(argument_connections.begin(), argument_connections.end(),
		std::back_inserter(arguments),
		[](const std::pair<llvm::Type*, std::string>& p) { return p.first; });

	// make these pointers
	std::transform(ret->begin(), ret->end(), std::back_inserter(arguments),
		[](llvm::Type* t) { return llvm::PointerType::get(t, 0); });

	llvm::Function* f = llvm::Function::Create(
		llvm::FunctionType::get(llvm::IntegerType::getInt32Ty(mod->getContext()), arguments, false),
		llvm::GlobalValue::LinkageTypes::ExternalLinkage, graphName, mod);
	llvm::BasicBlock* block = llvm::BasicBlock::Create(mod->getContext(), graphName, f);

	// follow linked list
	NodeInstance* node = getEntryNode().first;
	unsigned execInputID =
		node->outputExecConnections[0].second;  // the exec connection to codegen from

	std::unordered_map<NodeInstance*, cache> nodeCache;

	auto outputNode = getNodesWithType("lang", "exit")[0].first;

	// do entry, it's special
	auto& outputs = nodeCache[node].outputs;
	auto idx = 0ull;
	for (auto& arg : f->getArgumentList()) {
		if (idx < node->type->dataOutputs.size()) {
			arg.setName(node->type->dataOutputs[idx].second);
		} else {
			arg.setName(outputNode->type->dataInputs[idx - node->type->dataOutputs.size()].second);
		}
		outputs.push_back(&arg);
		++idx;
	}

	codegenHelper(node, execInputID, block, f, nodeCache);

	return res;
}

std::pair<NodeInstance*, size_t> GraphFunction::getEntryNode() const noexcept
{
	auto matching = getNodesWithType("lang", "entry");

	if (matching.size() == 1) {
		return matching[0];
	}
	return {nullptr, ~0};
}

std::vector<std::pair<NodeInstance*, size_t>> GraphFunction::getNodesWithType(
	const char* module, const char* name) const noexcept
{
	auto typeFinder = [&](
		auto& node) { return node->type->module == module && node->type->name == name; };

	std::vector<std::pair<NodeInstance*, size_t>> ret;
	auto iter = std::find_if(nodes.begin(), nodes.end(), typeFinder);
	while (iter != nodes.end()) {
		ret.emplace_back(iter->get(), std::distance(nodes.begin(), iter));

		iter = std::find_if(iter + 1, nodes.end(), typeFinder);
	}

	return ret;
}

boost::optional<std::vector<llvm::Type*>> GraphFunction::getReturnTypes() const noexcept
{
	auto matching = getNodesWithType("lang", "exit");

	if (matching.size() == 0) return {};

	auto& types = matching[0].first->type->dataInputs;

	// make sure they are all the same
	if (!std::all_of(matching.begin(), matching.end(), [&](auto pair) {
			return std::equal(types.begin(), types.end(), pair.first->type->dataInputs.begin());
		})) {
		return {};
	}

	std::vector<llvm::Type*> ret;
	ret.resize(types.size());
	std::transform(types.begin(), types.end(), ret.begin(), [](auto pair) { return pair.first; });

	return ret;
}

NodeInstance* GraphFunction::insertNode(std::unique_ptr<NodeType> type, float x, float y)
{
	auto ptr = std::make_unique<NodeInstance>(std::move(type), x, y);

	nodes.push_back(std::move(ptr));

	return nodes[nodes.size() - 1].get();
}
