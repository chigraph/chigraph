#include "chig/GraphFunction.hpp"
#include "chig/NodeType.hpp"
#include "chig/NodeInstance.hpp"

#include <llvm/AsmParser/Parser.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/raw_ostream.h>
#include <unordered_map>

using namespace chig;

GraphFunction::GraphFunction(Context& context, std::string name)
	: graphName{std::move(name)}, owningContext{&context}
{
}

GraphFunction::~GraphFunction() = default;

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
	if (data.find("nodes") == data.end() || !data["nodes"].is_object()) {
		res.add_entry("E5", "JSON in graph doesn't have nodes object", {});
		return res;
	}

	for (auto nodeiter = data["nodes"].begin(); nodeiter != data["nodes"].end(); ++nodeiter) {
		auto node = nodeiter.value();
		std::string nodeid = nodeiter.key();
		if (node.find("type") == node.end() || !node.find("type")->is_string()) {
			res.add_entry("E6", "Node doesn't have a \"type\" string", {{"nodeid", nodeid}});
			return res;
		}
		std::string fullType = node["type"];
		size_t colonId = fullType.find(':');
		std::string moduleName = fullType.substr(0, colonId);
		std::string typeName = fullType.substr(colonId + 1);

		if (colonId == std::string::npos || moduleName.size() == 0 || typeName.size() == 0) {
			res.add_entry("E7", "Incorrect qualified module name (should be module:type)", {{"nodeid", nodeid}, {"Requested Qualified Name", fullType}});
			return res;
		}

		if (node.find("data") == node.end()) {
			res.add_entry("E9", "Node doens't have a data section", {"nodeid", nodeid});
			return res;
		}

		std::unique_ptr<NodeType> nodeType;
		res += context.getNodeType(moduleName.c_str(), typeName.c_str(), node["data"], &nodeType);
		if(!res) { continue; }

		auto testIter = node.find("location");
		if (testIter != node.end()) {
			// make sure it is the right size
			if (!testIter.value().is_array()) {
				res.add_entry(
					"E10", "Node doesn't have a location that is an array.", {{"nodeid", nodeid}});
				continue;
			}

			if (testIter.value().size() != 2) {
				res.add_entry("E11", "Node doesn't have a location that is an array of size 2.",
					{{"nodeid", nodeid}});
				continue;
			}
		} else {
			res.add_entry("E12", "Node doesn't have a location.", {{"nodeid", nodeid}});
			continue;
		}

		ret->insertNode(std::move(nodeType), node["location"][0], node["location"][0], nodeid);
	}

	size_t connID = 0;
	// read the connections
	{
		auto connIter = data.find("connections");
		if(connIter == data.end() || !connIter->is_array()) {
			res.add_entry("E13", "No connections array in function", {});
			return res;
		}
		for (auto& connection : data["connections"]) {
			if(connection.find("type") == connection.end() || !connection.find("type")->is_string()) {
				res.add_entry("E14", "No type string in connection", {"connectionid", connID});
				continue;
			}
			std::string type = connection["type"];
			bool isData = type == "data";
			// it either has to be "data" or "exec"
			if (!isData && type != "exec") {
				res.add_entry("E15", "Unrecognized connection type",
					{{"connectionid", connID}, {"Found Type", type}});
				continue;
			}

			if (connection.find("input") == connection.end()) {
				res.add_entry("E16", "No input element in connection",  {{"connectionid", connID}});
				continue;
			}
			if (!connection.find("input")->is_array() || connection.find("input")->size() != 2 ||
				!connection.find("input")->operator[](0).is_string() ||
				!connection.find("input")->operator[](1).is_number_integer()
			) {
				res.add_entry("E17", "Incorrect connection input format, must be an array of of a string (node id) and int (connection id)", {{"connectionid", connID}, {"Requested Type", *connection.find("input")}});
				continue;
			}
			std::string InputNodeID = connection["input"][0];
			int InputConnectionID = connection["input"][1];


			if (connection.find("output") == connection.end()) {
				res.add_entry("E18", "No output element in connection",  {{"connectionid", connID}});
				continue;
			}
			if (!connection.find("output")->is_array() || connection.find("output")->size() != 2 ||
				!connection.find("output")->operator[](0).is_string() ||
				!connection.find("output")->operator[](1).is_number_integer()
			) {
				res.add_entry("E19", "Incorrect connection output format, must be an array of a string (node id) and int (connection id)", {{"connectionid", connID}, {"Requested Type", *connection.find("output")}});
				continue;
			}
			std::string OutputNodeID = connection["output"][0];
			int OutputConnectionID = connection["output"][1];

			// make sure the nodes exist
			if (ret->nodes.find(InputNodeID) == ret->nodes.end()) {
				res.add_entry("E20", "Input node for connection doesn't exist",
					{{"connectionid", connID}, {"Requested Node", InputNodeID}});
				continue;
			}
			if (ret->nodes.find(OutputNodeID) == ret->nodes.end()) {
				res.add_entry("E21", "Output node for connection doesn't exist",
					{{"connectionid", connID}, {"Requested Node", OutputNodeID}});
				continue;
			}

			// connect
			// these functions do bounds checking, it's okay
			if (isData) {
				res += connectData(*ret->nodes[InputNodeID], InputConnectionID, *ret->nodes[OutputNodeID],
					OutputConnectionID);
			} else {
				res += connectExec(*ret->nodes[InputNodeID], InputConnectionID, *ret->nodes[OutputNodeID],
					OutputConnectionID);
			}

			++connID;
		}
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
	jsonNodes = nlohmann::json::object();  // make sure even if it's empty it's an object
	auto& jsonConnections = jsonData["connections"];
	jsonConnections = nlohmann::json::array();  // make sure even if it's empty it's an aray

	for (auto nodeIter = nodes.begin(); nodeIter != nodes.end(); ++nodeIter) {
		auto& node = nodeIter->second;
		std::string nodeID = nodeIter->first;

		nlohmann::json nodeJson = node->type->toJSON();
		jsonNodes[nodeID] = {{"type", node->type->module + ':' + node->type->name},
			{"location", {node->x, node->y}}, {"data", nodeJson}};
		// add its connections. Just out the outputs to avoid duplicates

		// add the exec outputs
		for (auto conn_id = 0ull; conn_id < node->outputExecConnections.size(); ++conn_id) {
			auto& conn = node->outputExecConnections[conn_id];
			// if there is actually a connection
			if (conn.first) {

				jsonConnections.push_back({{"type", "exec"}, {"input", {nodeID, conn_id}},
					{"output", {conn.first->id, conn.second}}});
			}
		}

		// add the data outputs
		for (auto conn_id = 0ull; conn_id < node->outputDataConnections.size(); ++conn_id) {
			auto& connsforid = node->outputDataConnections[conn_id];
			for(auto& conn : connsforid) {
				// if there is actually a connection
				if (conn.first) {

					jsonConnections.push_back({{"type", "data"}, {"input", {nodeID, conn_id}},
						{"output", {conn.first->id, conn.second}}});
				}
			}
		}
	}

	return res;
}

struct cache {
	std::vector<llvm::Value*> outputs;
};

// Codegens a single input to a node
void codegenHelper(NodeInstance* node, unsigned execInputID, llvm::BasicBlock* block, llvm::BasicBlock* allocblock,
	llvm::Module* mod, llvm::Function* f, std::unordered_map<NodeInstance*, cache>& nodeCache, Result& res)
{
	llvm::IRBuilder<> builder(block);
	// get input values
	std::vector<llvm::Value*> inputParams;
	
	size_t inputID = 0;
	for (auto& param : node->inputDataConnections) {
		// TODO: error handling

		if(!param.first) {
			res.add_entry("E232", "No data input to node", {{"nodeid", node->id}, {"input ID", inputID}});
			
			return;
		}
		
		auto cacheiter = nodeCache.find(param.first);
		
		if(cacheiter == nodeCache.end()) {
			res.add_entry("E231", "Failed to find in cache", {{"nodeid", param.first->id}});
			
			return;
		}
		
		auto& cacheObject = cacheiter->second;
        if(param.second >= cacheObject.outputs.size()) {
			res.add_entry("E232", "No data input to node", {{"nodeid", node->id}, {"input ID", inputID}});
			
			return;
		}
		
		// get pointers to the objects
		auto value = cacheObject.outputs[param.second];
		// dereference
		inputParams.push_back(builder.CreateLoad(value, ""));  // TODO: pass ptr to value
		
		++inputID;
	}

	auto entryBlock = &f->getEntryBlock();
	llvm::IRBuilder<> entryBuilder(entryBlock);
	llvm::IRBuilder<> allocBuilder(allocblock);

	// create outputs
	auto& outputCache = nodeCache[node].outputs;
	for (auto& output : node->type->dataOutputs) {
		// TODO: research address spaces
		llvm::AllocaInst* alloc =
			allocBuilder.CreateAlloca(output.first, nullptr, output.second);  // TODO: name
		alloc->setName(output.second);
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
	node->type->codegen(execInputID, mod, f, inputParams, block, outputBlocks);

	// codegen for all outputs
	for (auto idx = 0ull; idx < node->outputExecConnections.size(); ++idx) {
		auto& output = node->outputExecConnections[idx];
		if (output.first)
			codegenHelper(output.first, output.second, outputBlocks[idx], allocblock, mod, f, nodeCache, res);
	}
}

Result GraphFunction::compile(llvm::Module* mod, llvm::Function** ret_func) const
{
	Result res;

	auto entry = getEntryNode();
	if(!entry) {
		res.add_entry("EUKN", "No entry node", {});
		return res;
	}

	const auto& argument_connections =
		getEntryNode()->type->dataOutputs;  // ouptuts from entry are arguments

	// get return types;
	auto ret = getReturnTypes();
	if (!ret) {
		res.add_entry("E34", "No return type in function", {});
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
	llvm::BasicBlock* allocblock = llvm::BasicBlock::Create(mod->getContext(), graphName, f);
	llvm::BasicBlock* block = llvm::BasicBlock::Create(mod->getContext(), graphName, f);
	auto blockcpy = block;

	// follow "linked list"
	NodeInstance* node = getEntryNode();
	unsigned execInputID =
		node->outputExecConnections[0].second;  // the exec connection to codegen from

	std::unordered_map<NodeInstance*, cache> nodeCache;

	auto outputNode = getNodesWithType("lang", "exit")[0];

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

	codegenHelper(node, execInputID, block, allocblock, mod, f, nodeCache, res);

	llvm::IRBuilder<> allocbuilder(allocblock);
	allocbuilder.CreateBr(blockcpy);

	return res;
}

NodeInstance* GraphFunction::getEntryNode() const noexcept
{
	auto matching = getNodesWithType("lang", "entry");

	if (matching.size() == 1) {
		return matching[0];
	}
	return nullptr;
}

std::vector<NodeInstance*> GraphFunction::getNodesWithType(
	const char* module, const char* name) const noexcept
{
	auto typeFinder = [&](
		auto& pair) { return pair.second->type->module == module && pair.second->type->name == name; };

	std::vector<NodeInstance*> ret;
	auto iter = std::find_if(nodes.begin(), nodes.end(), typeFinder);
	while (iter != nodes.end()) {
		ret.emplace_back(iter->second.get());

		std::advance(iter, 1); // don't process the same one twice!
		iter = std::find_if(iter, nodes.end(), typeFinder);
	}

	return ret;
}

boost::optional<std::vector<llvm::Type*>> GraphFunction::getReturnTypes() const noexcept
{
	auto matching = getNodesWithType("lang", "exit");

	if (matching.size() == 0) return {};

	auto& types = matching[0]->type->dataInputs;

	// make sure they are all the same
	if (!std::all_of(matching.begin(), matching.end(), [&](auto pair) {
			return std::equal(types.begin(), types.end(), pair->type->dataInputs.begin());
		})) {
		return {};
	}

	std::vector<llvm::Type*> ret;
	ret.resize(types.size());
	std::transform(types.begin(), types.end(), ret.begin(), [](auto pair) { return pair.first; });

	return ret;
}

NodeInstance* GraphFunction::insertNode(std::unique_ptr<NodeType> type, float x, float y, const std::string& id)
{
	auto ptr = std::make_unique<NodeInstance>(std::move(type), x, y, id);


	auto emplaced = nodes.emplace(id, std::move(ptr)).first;

	return emplaced->second.get();
}
