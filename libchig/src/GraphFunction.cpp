#include "chig/GraphFunction.hpp"
#include "chig/NodeType.hpp"
#include "chig/NodeInstance.hpp"

#include <llvm/AsmParser/Parser.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/raw_ostream.h>
#include <unordered_map>

using namespace chig;

GraphFunction::GraphFunction(Context& context_, std::string name)
	: graphName{std::move(name)}, context{&context_}
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

	ret->source = data;

	return res;
}

Result GraphFunction::toJSON(nlohmann::json* toFill) const
{
	Result res;

	*toFill = nlohmann::json{};
	auto& jsonData = *toFill;

	jsonData["type"] = "function";
	jsonData["name"] = graphName;

	res += graph.toJson(toFill);

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

	auto outputNode = graph.getNodesWithType("lang", "exit")[0];

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
	auto matching = graph.getNodesWithType("lang", "entry");

	if (matching.size() == 1) {
		return matching[0];
	}
	return nullptr;
}
boost::optional<std::vector<llvm::Type*>> GraphFunction::getReturnTypes() const noexcept
{
	auto matching = graph.getNodesWithType("lang", "exit");

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

Result GraphFunction::loadGraph() {
	Result res;
	graph = Graph(*context, source, res);

	return res;
}

