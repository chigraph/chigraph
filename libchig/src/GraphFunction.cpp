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
	
	if(data.find("inputs") == data.end() || !data["inputs"].is_array()) {
		res.add_entry("E43", "JSON in graph doesn't have an inputs array", {});
		return res;
	}
	
	
	// construct it
	*ret_ptr = std::make_unique<GraphFunction>(context, name);
	auto& ret = *ret_ptr;
	
	for(auto param : data["inputs"]) {
		
		for(auto iter = param.begin(); iter != param.end(); ++iter) {
			
			std::string qualifiedType = iter.value();
			std::string docString = iter.key();
			
			std::string module = qualifiedType.substr(0, qualifiedType.find(':'));
			std::string name = qualifiedType.substr(qualifiedType.find(':') + 1);
			
			llvm::Type* ty;
			res += context.getType(module.c_str(), name.c_str(), &ty);
			
			if(!res) return res;
			
			ret->inputs.emplace_back(ty, docString);
		}
		
	}

	if(data.find("outputs") == data.end() || !data["outputs"].is_array()) {
		res.add_entry("E44", "JSON in graph doesn't have an outputs array", {});
		return res;
	}
	
	for(auto param : data["outputs"]) {
		
		for(auto iter = param.begin(); iter != param.end(); ++iter) {
			
			std::string qualifiedType = iter.value();
			std::string docString = iter.key();
			
			std::string module = qualifiedType.substr(0, qualifiedType.find(':'));
			std::string name = qualifiedType.substr(qualifiedType.find(':') + 1);
			
			llvm::Type* ty;
			res += context.getType(module.c_str(), name.c_str(), &ty);
			
			if(!res) return res;
			
			ret->outputs.emplace_back(ty, docString);
		}
		
	}
	

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
	auto printmod = [&](){std::cout << node->type->context->stringifyModule(mod);};
	
	llvm::IRBuilder<> builder(block);
	// get input values
	std::vector<llvm::Value*> inputParams;
	
	size_t inputID = 0;
	for (auto& param : node->inputDataConnections) {
		// TODO: error handling

		if(!param.first) {
			res.add_entry("EUKN", "No data input to node", {{"nodeid", node->id}, {"input ID", inputID}});
			
			return;
		}
		
		auto cacheiter = nodeCache.find(param.first);
		
		if(cacheiter == nodeCache.end()) {
			res.add_entry("EUKN", "Failed to find in cache", {{"nodeid", param.first->id}});
			
			return;
		}
		
		auto& cacheObject = cacheiter->second;
        if(param.second >= cacheObject.outputs.size()) {
			res.add_entry("EUKN", "No data input to node", {{"nodeid", node->id}, {"input ID", inputID}});
			
			return;
		}
		
		// get pointers to the objects
		auto value = cacheObject.outputs[param.second];
		// dereference
		inputParams.push_back(builder.CreateLoad(value, node->type->dataInputs[inputID].second));  // TODO: pass ptr to value
		
		++inputID;
	}

	llvm::IRBuilder<> allocBuilder(allocblock);

	// create outputs
	auto& outputCache = nodeCache[node].outputs;
	for (auto& output : node->type->dataOutputs) {
		// TODO: research address spaces
		llvm::AllocaInst* alloc =
			allocBuilder.CreateAlloca(output.first, nullptr, output.second); 
		alloc->setName(output.second);
		outputCache.push_back(alloc);
	}

	// combine
	std::copy(outputCache.begin(), outputCache.end(), std::back_inserter(inputParams));

	// create output blocks
	std::vector<llvm::BasicBlock*> outputBlocks;
	for (auto idx = 0ull; idx < node->outputExecConnections.size(); ++idx) {
		auto outBlock = llvm::BasicBlock::Create(f->getContext(), node->type->execOutputs[idx], f);
		outputBlocks.push_back(outBlock);
		
		outBlock->setName("node_" + node->outputExecConnections[idx].first->id);
		
	}

	// codegen
	res += node->type->codegen(execInputID, mod, f, inputParams, block, outputBlocks);
	if(!res) {
		return;
	}
	
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

	llvm::Function* f = llvm::cast<llvm::Function>(mod->getOrInsertFunction(graphName, getFunctionType())); // TODO: name mangling
	llvm::BasicBlock* allocblock = llvm::BasicBlock::Create(mod->getContext(), "alloc", f);
	llvm::BasicBlock* block = llvm::BasicBlock::Create(mod->getContext(), graphName + "_entry", f);
	auto blockcpy = block;

	// follow "linked list"
	NodeInstance* node = getEntryNode();
	unsigned execInputID =
		node->outputExecConnections[0].second;  // the exec connection to codegen from

	std::unordered_map<NodeInstance*, cache> nodeCache;

	NodeInstance* outputNode;
	// get output node 
	{
		auto outputNodes = graph.getNodesWithType("lang", "exit");
		
		if(outputNodes.empty()) {
			res.add_entry("EUKN", "No output nodes in graph", {{"Graph Name", graphName}});
			return res;
		}
		
		outputNode = outputNodes[0];
	}

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

Result GraphFunction::loadGraph() {
	Result res;
	graph = Graph(*context, source, res);

	return res;
}

llvm::FunctionType* GraphFunction::getFunctionType() const {

	std::vector<llvm::Type*> arguments;
	arguments.reserve(inputs.size());
	std::transform(inputs.begin(), inputs.end(),
		std::back_inserter(arguments),
		[](const std::pair<llvm::Type*, std::string>& p) { return p.first; });

	// make these pointers
	std::transform(outputs.begin(), outputs.end(), std::back_inserter(arguments),
		[](auto& tyanddoc) { return llvm::PointerType::get(tyanddoc.first, 0); });

	return llvm::FunctionType::get(llvm::IntegerType::getInt32Ty(context->llcontext), arguments, false);
	
}
