#include "chig/GraphFunction.hpp"
#include "chig/JsonModule.hpp"
#include "chig/NameMangler.hpp"
#include "chig/NodeInstance.hpp"
#include "chig/NodeType.hpp"

#include <llvm/AsmParser/Parser.h>
#include <llvm/IR/Type.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/raw_ostream.h>
#include <unordered_map>

using namespace chig;

GraphFunction::GraphFunction(JsonModule& mod, gsl::cstring_span<> name,
	std::vector<std::pair<DataType, std::string>> ins,
	std::vector<std::pair<DataType, std::string>> outs)
	: mModule{&mod},
	  mContext{&mod.context()},
	  mName{gsl::to_string(name)},
	  mInputs(std::move(ins)),
	  mOutputs(std::move(outs)),
	  mSource()
{
}

GraphFunction::~GraphFunction() = default;

Result GraphFunction::fromJSON(
	JsonModule& module, const nlohmann::json& data, std::unique_ptr<GraphFunction>* ret_func)
{
	Result res = {};

	if (!data.is_object()) {
		res.addEntry("E1", "Graph json isn't a JSON object", {});
		return res;
	}
	// make sure it has a type element
	if (data.find("type") == data.end()) {
		res.addEntry("E2", R"(JSON in graph doesn't have a "type" element)", {});
		return res;
	}
	if (data["type"] != "function") {
		res.addEntry("E3", "JSON in graph doesn't have a function type", {});
		return res;
	}
	// make sure there is a name
	if (data.find("name") == data.end()) {
		res.addEntry("E4", "JSON in graph doesn't have a name parameter", {});
		return res;
	}
	std::string name = data["name"];

	if (data.find("inputs") == data.end() || !data["inputs"].is_array()) {
		res.addEntry("E43", "JSON in graph doesn't have an inputs array", {});
		return res;
	}

	std::vector<std::pair<DataType, std::string>> inputs;
	for (auto param : data["inputs"]) {
		for (auto iter = param.begin(); iter != param.end(); ++iter) {
			std::string qualifiedType = iter.value();
			std::string docString = iter.key();

			std::string moduleName, name;
			std::tie(moduleName, name) = parseColonPair(qualifiedType);

			DataType ty;
			res += module.context().typeFromModule(moduleName, name, &ty);

			if (!res) {
				return res;
			}

			inputs.emplace_back(ty, docString);
		}
	}

	if (data.find("outputs") == data.end() || !data["outputs"].is_array()) {
		res.addEntry("E44", "JSON in graph doesn't have an outputs array", {});
		return res;
	}

	std::vector<std::pair<DataType, std::string>> outputs;
	for (auto param : data["outputs"]) {
		for (auto iter = param.begin(); iter != param.end(); ++iter) {
			std::string qualifiedType = iter.value();
			std::string docString = iter.key();

			std::string moduleName, name;
			std::tie(moduleName, name) = parseColonPair(qualifiedType);

			DataType ty;
			res += module.context().typeFromModule(moduleName, name, &ty);

			if (!res) {
				return res;
			}

			outputs.emplace_back(ty, docString);
		}
	}

	// construct it
	*ret_func =
		std::make_unique<GraphFunction>(module, name, std::move(inputs), std::move(outputs));
	auto& ret = *ret_func;

	ret->mSource = data;

	return res;
}

Result GraphFunction::toJSON(nlohmann::json* toFill) const
{
	Result res;

	*toFill = nlohmann::json{};
	auto& jsonData = *toFill;

	jsonData["type"] = "function";
	jsonData["name"] = name();

	auto& inputsjson = jsonData["inputs"];
	inputsjson = nlohmann::json::array();

	for (auto& in : inputs()) {
		inputsjson.push_back({{in.second, in.first.qualifiedName()}});
	}

	auto& outputsjson = jsonData["outputs"];
	outputsjson = nlohmann::json::array();

	for (auto& out : outputs()) {
		outputsjson.push_back({{out.second, out.first.qualifiedName()}});
	}

	res += graph().toJson(toFill);

	return res;
}

struct cache {
	std::vector<llvm::Value*> outputs;
};

// Codegens a single input to a node
void codegenHelper(NodeInstance* node, unsigned execInputID, llvm::BasicBlock* block,
	llvm::BasicBlock* allocblock, llvm::Module* mod, llvm::Function* f,
	std::unordered_map<NodeInstance*, cache>& nodeCache, Result& res)
{
	llvm::IRBuilder<> builder(block);

	// get inputs and outputs
	std::vector<llvm::Value*> io;

	// add inputs
	{
		size_t inputID = 0;
		for (auto& param : node->inputDataConnections) {
			// make sure everything is A-OK
			if (param.first == nullptr) {
				res.addEntry("EUKN", "No data input to node",
					{{"nodeid", node->id()}, {"input ID", inputID}});

				return;
			}

			auto cacheiter = nodeCache.find(param.first);

			if (cacheiter == nodeCache.end()) {
				res.addEntry("EUKN", "Failed to find in cache", {{"nodeid", param.first->id()}});

				return;
			}

			auto& cacheObject = cacheiter->second;
			if (param.second >= cacheObject.outputs.size()) {
				res.addEntry("EUKN", "No data input to node",
					{{"nodeid", node->id()}, {"input ID", inputID}});

				return;
			}

			// get pointers to the objects
			auto value = cacheObject.outputs[param.second];
			// dereference
			io.push_back(builder.CreateLoad(
				value, node->type().dataInputs()[inputID].second));  // TODO: pass ptr to value

			// make sure it's the right type
			if (io[io.size() - 1]->getType() !=
				node->type().dataInputs()[inputID].first.llvmType()) {
				res.addEntry("EINT", "Internal codegen error: unexpected type in cache.",
					{{"Expected LLVM type",
						 stringifyLLVMType(node->type().dataInputs()[inputID].first.llvmType())},
						{"Found type", stringifyLLVMType(io[io.size() - 1]->getType())},
						{"Node ID", node->id()}, {"Input ID", inputID}});
			}

			++inputID;
		}
	}

	// get outputs
	{
		llvm::IRBuilder<> allocBuilder(allocblock);

		// create outputs
		auto& outputCache = nodeCache[node].outputs;

		for (auto& output : node->type().dataOutputs()) {
			// TODO: research address spaces
			llvm::AllocaInst* alloc =
				allocBuilder.CreateAlloca(output.first.llvmType(), nullptr, output.second);
			alloc->setName(output.second);
			outputCache.push_back(alloc);
			io.push_back(alloc);

			// make sure the type is right
			if (llvm::PointerType::get(output.first.llvmType(), 0) != alloc->getType()) {
				res.addEntry("EINT",
					"Internal codegen error: unexpected type returned from alloca.",
					{{"Expected LLVM type",
						 stringifyLLVMType(llvm::PointerType::get(output.first.llvmType(), 0))},
						{"Yielded type", stringifyLLVMType(alloc->getType())},
						{"Node ID", node->id()}});
			}
		}
	}

	// create output blocks
	std::vector<llvm::BasicBlock*> outputBlocks;
	std::vector<llvm::BasicBlock*> unusedBlocks;
	for (auto idx = 0ull; idx < node->outputExecConnections.size(); ++idx) {
		auto outBlock =
			llvm::BasicBlock::Create(f->getContext(), node->type().execOutputs()[idx], f);
		outputBlocks.push_back(outBlock);
		if (node->outputExecConnections[idx].first != nullptr) {
			outBlock->setName("node_" + node->outputExecConnections[idx].first->id());
		} else {
			unusedBlocks.push_back(outBlock);
		}
	}

	// codegen
	res += node->type().codegen(execInputID, mod, f, io, block, outputBlocks);
	if (!res) {
		return;
	}

	// TODO: sequence nodes
	for (auto& bb : unusedBlocks) {
		llvm::IRBuilder<> builder(bb);

		builder.CreateRet(
			llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(mod->getContext()), 0, true));
	}

	// recurse!
	for (auto idx = 0ull; idx < node->outputExecConnections.size(); ++idx) {
		auto& output = node->outputExecConnections[idx];
		if (output.first != nullptr) {
			codegenHelper(
				output.first, output.second, outputBlocks[idx], allocblock, mod, f, nodeCache, res);
		}
	}
}

Result GraphFunction::compile(llvm::Module* mod, llvm::Function** ret_func) const
{
	Result res;

	auto entry = entryNode();
	if (entry == nullptr) {
		res.addEntry("EUKN", "No entry node", {});
		return res;
	}

	NodeInstance* exitNode;
	// get output node
	{
		auto outputNodes = graph().nodesWithType("lang", "exit");

		if (outputNodes.empty()) {
			res.addEntry("EUKN", "No output nodes in graph", {{"Graph Name", name()}});
			return res;
		}

		exitNode = outputNodes[0];
	}

	// make sure that the entry node has the functiontype
	if (!std::equal(inputs().begin(), inputs().end(), entry->type().dataOutputs().begin())) {
		nlohmann::json inFunc = nlohmann::json::array();
		for (auto& in : inputs()) {
			inFunc.push_back({{in.second, in.first.qualifiedName()}});
		}

		nlohmann::json inEntry = nlohmann::json::array();
		for (auto& in :
			entry->type().dataOutputs()) {  // outputs to entry are inputs to the function
			inEntry.push_back({{in.second, in.first.qualifiedName()}});
		}

		res.addEntry("EUKN", "Inputs to function doesn't match function inputs",
			{{"Function Inputs", inFunc}, {"Entry Inputs", inEntry}});
		return res;
	}

	// make sure that the entry node has the functiontype
	if (!std::equal(outputs().begin(), outputs().end(), exitNode->type().dataInputs().begin())) {
		nlohmann::json outFunc = nlohmann::json::array();
		for (auto& out : outputs()) {
			outFunc.push_back({{out.second, out.first.qualifiedName()}});
		}

		nlohmann::json outEntry = nlohmann::json::array();
		for (auto& out : exitNode->type().dataInputs()) {
			// inputs to the exit are outputs to the function
			outEntry.push_back({{out.second, out.first.qualifiedName()}});
		}

		res.addEntry("EUKN", "Outputs to function doesn't match function exit",
			{{"Function Outputs", outFunc}, {"Entry Outputs", outEntry}});
		return res;
	}

	llvm::Function* f = llvm::cast<llvm::Function>(mod->getOrInsertFunction(
		mangleFunctionName(module().fullName(), name()), functionType()));  // TODO: name mangling
	llvm::BasicBlock* allocblock = llvm::BasicBlock::Create(mod->getContext(), "alloc", f);
	llvm::BasicBlock* block = llvm::BasicBlock::Create(mod->getContext(), name() + "_entry", f);
	auto blockcpy = block;

	// follow "linked list"
	unsigned execInputID =
		entry->outputExecConnections[0].second;  // the exec connection to codegen from

	std::unordered_map<NodeInstance*, cache> nodeCache;

	// set argument names
	auto idx = 0ull;
	for (auto& arg : f->getArgumentList()) {
		if (idx < entry->type().dataOutputs().size()) {
			// dataOutputs to entry are inputs to the function
			arg.setName(entry->type()
							.dataOutputs()[idx]
							.second);  // it starts with inputs, which are outputs to entry
		} else {
			arg.setName(exitNode->type()
							.dataInputs()[idx - entry->type().dataOutputs().size()]
							.second);  // then the outputs, which are inputs to exit
		}
		++idx;
	}

	codegenHelper(entry, execInputID, block, allocblock, mod, f, nodeCache, res);

	llvm::IRBuilder<> allocbuilder(allocblock);
	allocbuilder.CreateBr(blockcpy);

	*ret_func = f;
	return res;
}

NodeInstance* GraphFunction::entryNode() const noexcept
{
	auto matching = graph().nodesWithType("lang", "entry");

	if (matching.size() == 1) {
		// make sure it has the same signature as the method
		if (!std::equal(inputs().begin(), inputs().end(), matching[0]->type().dataOutputs().begin(),
				matching[0]->type().dataOutputs().end())) {
			return nullptr;
		}
		return matching[0];
	}
	return nullptr;
}

Result GraphFunction::loadGraph()
{
	Result res;
	mGraph = Graph(context(), mSource, res);

	return res;
}

llvm::FunctionType* GraphFunction::functionType() const
{
	std::vector<llvm::Type*> arguments;
	arguments.reserve(inputs().size());
	std::transform(inputs().begin(), inputs().end(), std::back_inserter(arguments),
		[](const std::pair<DataType, std::string>& p) { return p.first.llvmType(); });

	// make these pointers
	std::transform(outputs().begin(), outputs().end(), std::back_inserter(arguments),
		[](auto& tyanddoc) { return llvm::PointerType::get(tyanddoc.first.llvmType(), 0); });

	return llvm::FunctionType::get(
		llvm::IntegerType::getInt32Ty(context().llvmContext()), arguments, false);
}
