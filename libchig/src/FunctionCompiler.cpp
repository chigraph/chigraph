/// \file FunctionCompiler.cpp

#include "chig/FunctionCompiler.hpp"
#include "chig/FunctionValidator.hpp"
#include "chig/GraphFunction.hpp"
#include "chig/GraphModule.hpp"
#include "chig/NameMangler.hpp"
#include "chig/NodeInstance.hpp"

#include <boost/bimap.hpp>
#include <boost/dynamic_bitset.hpp>
#include <boost/range/join.hpp>

#include <unordered_map>

#include <llvm/IR/IRBuilder.h>

namespace chig {

namespace {

struct Cache {
	// only used for pure nodes
	NodeInstance*                  lastNodeCodegenned = nullptr;
	std::vector<llvm::Value*>      outputs;
	std::vector<llvm::BasicBlock*> inputBlock;  // one for each exec input
};

struct codegenMetadata {
	llvm::BasicBlock*   allocBlock;
	llvm::DIBuilder*    dbuilder;
	llvm::DISubprogram* diFunc;
	std::unordered_map<NodeInstance*, Cache>               nodeCache;
	boost::bimap<unsigned, NodeInstance*>                  nodeLocations;
	std::unordered_map<std::string, std::shared_ptr<void>> compileCache;
};

/// \return The output connections that need codegen and the output blocks
std::pair<boost::dynamic_bitset<>, std::vector<llvm::BasicBlock*>> codegenNode(
    NodeInstance* node, llvm::BasicBlock* pureTerminator, NodeInstance* lastImpure,
    unsigned execInputID, llvm::BasicBlock* block, codegenMetadata& data, Result& res) {
	llvm::IRBuilder<> pureBuilder(block);
	auto              f   = data.allocBlock->getParent();
	auto              mod = data.allocBlock->getModule();

	auto codeBlock =
	    llvm::BasicBlock::Create(node->context().llvmContext(), node->id() + "_code", f);
	llvm::IRBuilder<> builder(codeBlock);

	// get inputs and outputs
	std::vector<llvm::Value*> io;

	// add inputs
	{
		// process pures -- call them again if they're out of date
		{
			std::vector<NodeInstance*>     pureDependencies;
			std::vector<llvm::BasicBlock*> pureBBs;
			for (const auto& param : node->inputDataConnections) {
				if (param.first->type().pure()) {
					auto        impure = node->type().pure() ? lastImpure : node;
					const auto& cache  = data.nodeCache[param.first];
					if (cache.lastNodeCodegenned != impure) {
						pureDependencies.push_back(param.first);
						pureBBs.push_back(
						    llvm::BasicBlock::Create(node->context().llvmContext(),
						                             param.first->id() + "____" + impure->id(), f));
					}
				}
			}
			pureBBs.push_back(codeBlock);

			for (auto idx = 0ull; idx < pureDependencies.size(); ++idx) {
				auto pureNode            = pureDependencies[idx];
				auto pureCodegenInto     = pureBBs[idx];
				auto pureTerminatorBlock = pureBBs[idx + 1];

				auto impure = node->type().pure() ? lastImpure : node;
				codegenNode(pureNode, pureTerminatorBlock, impure, execInputID, pureCodegenInto,
				            data, res);
			}
			if (pureDependencies.empty()) {
				pureBuilder.CreateBr(codeBlock);
			} else {
				pureBuilder.CreateBr(pureBBs[0]);
			}
		}

		size_t inputID = 0;
		for (auto& param : node->inputDataConnections) {
			// make sure everything is A-OK
			if (param.first == nullptr) {
				res.addEntry("EUKN", "No data input to node",
				             {{"nodeid", node->id()},
				              {"input ID", inputID},
				              {"nodetype", node->type().qualifiedName()}});

				return {boost::dynamic_bitset<>{}, std::vector<llvm::BasicBlock*>{}};
			}

			auto cacheiter = data.nodeCache.find(param.first);
			if (cacheiter == data.nodeCache.end()) {
				res.addEntry("EUKN", "Failed to find in cache", {{"nodeid", param.first->id()}});

				return {boost::dynamic_bitset<>{}, std::vector<llvm::BasicBlock*>{}};
			}

			auto& cacheObject = cacheiter->second;
			if (param.second >= cacheObject.outputs.size()) {
				res.addEntry("EUKN", "No data input to node",
				             {{"nodeid", node->id()},
				              {"input ID", inputID},
				              {"nodetype", node->type().qualifiedName()}});

				return {boost::dynamic_bitset<>{}, std::vector<llvm::BasicBlock*>{}};
			}

			// get pointers to the objects
			auto value = cacheObject.outputs[param.second];
			// dereference
			io.push_back(builder.CreateLoad(value));

			// make sure it's the right type
			if (io[io.size() - 1]->getType() !=
			    node->type().dataInputs()[inputID].type.llvmType()) {
				res.addEntry(
				    "EINT", "Internal codegen error: unexpected type in cache.",
				    {{"Expected LLVM type",
				      stringifyLLVMType(node->type().dataInputs()[inputID].type.llvmType())},
				     {"Found type", stringifyLLVMType(io[io.size() - 1]->getType())},
				     {"Node ID", node->id()},
				     {"Input ID", inputID}});
			}
		}
	}

	// get outputs
	{
		llvm::IRBuilder<> allocBuilder(data.allocBlock);

		// create outputs
		auto& outputCache = data.nodeCache[node].outputs;
		outputCache.resize(node->type().dataOutputs().size(), nullptr);

		auto id = 0ull;
		for (auto& output : node->type().dataOutputs()) {
			if (outputCache[id] != nullptr) {
				io.push_back(outputCache[id]);
				++id;
				continue;
			}

			llvm::AllocaInst* alloc = allocBuilder.CreateAlloca(
			    output.type.llvmType(), nullptr, node->id() + "__" + std::to_string(id));
			outputCache[id] = alloc;
			io.push_back(alloc);

			// create debug info
			{
				// get type
				llvm::DIType* dType = output.type.debugType();

				// TODO(#63): better names
				auto debugVar = data.dbuilder->createAutoVariable(
				    data.diFunc, node->id() + "__" + std::to_string(id), data.diFunc->getFile(), 1,
				    dType);

				data.dbuilder->insertDeclare(alloc, debugVar, data.dbuilder->createExpression(),
				                             llvm::DebugLoc::get(1, 1, data.diFunc),
				                             data.allocBlock);
			}

			// make sure the type is right
			if (llvm::PointerType::get(output.type.llvmType(), 0) != alloc->getType()) {
				res.addEntry(
				    "EINT", "Internal codegen error: unexpected type returned from alloca.",
				    {{"Expected LLVM type",
				      stringifyLLVMType(llvm::PointerType::get(output.type.llvmType(), 0))},
				     {"Yielded type", stringifyLLVMType(alloc->getType())},
				     {"Node ID", node->id()}});
			}
			++id;
		}
	}

	// add this block to the cache
	if (node->type().qualifiedName() != "lang:entry") {
		data.nodeCache[node].inputBlock.resize(
		    node->type().pure() ? 1 : node->inputExecConnections.size(), nullptr);
		data.nodeCache[node].inputBlock[execInputID] = block;
	}

	// create output blocks
	std::vector<llvm::BasicBlock*> outputBlocks;
	std::vector<llvm::BasicBlock*> unusedBlocks;
	boost::dynamic_bitset<>
	    needsCodegen;  // this holds if each block needs to recurse of it it's already in the cache
	for (auto idx = 0ull; idx < node->outputExecConnections.size(); ++idx) {
		// see if we can find it in the cache
		NodeInstance* remoteNode;
		size_t        remotePortIdx;
		std::tie(remoteNode, remotePortIdx) = node->outputExecConnections[idx];

		auto remoteCache =
		    data.nodeCache[remoteNode];  // this will construct it if it doesn't exist
		if (remoteCache.inputBlock.size() > remotePortIdx &&
		    remoteCache.inputBlock[remotePortIdx] != nullptr) {
			// this means it's already in the cache
			outputBlocks.push_back(remoteCache.inputBlock[remotePortIdx]);
			needsCodegen.push_back(false);
		} else {
			auto outBlock =
			    llvm::BasicBlock::Create(f->getContext(), node->type().execOutputs()[idx], f);
			outputBlocks.push_back(outBlock);
			needsCodegen.push_back(true);  // these need codegen
			if (node->outputExecConnections[idx].first != nullptr) {
				outBlock->setName("node_" + node->outputExecConnections[idx].first->id());
			} else {
				unusedBlocks.push_back(outBlock);
			}
		}
	}
	if (node->type().pure()) { outputBlocks.push_back(pureTerminator); }

	// codegen
	res += node->type().codegen(
	    execInputID, llvm::DebugLoc::get(data.nodeLocations.right.at(node), 1, data.diFunc), io,
	    codeBlock, outputBlocks, data.compileCache);
	if (!res) { return {boost::dynamic_bitset<>{}, std::vector<llvm::BasicBlock*>{}}; }

	// TODO(#64): raise an error here instead
	for (auto& bb : unusedBlocks) {
		llvm::IRBuilder<> builder(bb);

		builder.CreateRet(
		    llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(mod->getContext()), 0, true));
	}

	data.nodeCache[node].lastNodeCodegenned = lastImpure;

	return {needsCodegen, outputBlocks};
}

/// \internal
/// Codegens a single input to a node
/// All of these nodes are garunteed to be impure
void codegenHelper(NodeInstance* node, unsigned execInputID, llvm::BasicBlock* block,
                   codegenMetadata& data, Result& res) {
	boost::dynamic_bitset<>        needsCodegen;
	std::vector<llvm::BasicBlock*> outputBlocks;
	std::tie(needsCodegen, outputBlocks) =
	    codegenNode(node, nullptr, nullptr, execInputID, block, data, res);

	if (!res) { return; }

	// recurse!
	for (auto idx = 0ull; idx < node->outputExecConnections.size(); ++idx) {
		auto& output = node->outputExecConnections[idx];
		if (output.first != nullptr && needsCodegen[idx]) {
			codegenHelper(output.first, output.second, outputBlocks[idx], data, res);
		}
	}
}

}  // anonymous namespace

Result compileFunction(const GraphFunction& func, llvm::Module* mod, llvm::DICompileUnit* debugCU,
                       llvm::DIBuilder& debugBuilder) {
	Expects(mod != nullptr);

	Result res;

	res += validateFunction(func);

	if (!res) { return res; }

	auto entry = func.entryNode();
	if (entry == nullptr) {
		res.addEntry("EUKN", "No entry node", {});
		return res;
	}

	// make sure that the entry node has the functiontype
	if (!std::equal(func.dataInputs().begin(), func.dataInputs().end(),
	                entry->type().dataOutputs().begin())) {
		nlohmann::json inFunc = nlohmann::json::array();
		for (auto& in : func.dataInputs()) {
			inFunc.push_back({{in.name, in.type.qualifiedName()}});
		}

		nlohmann::json inEntry = nlohmann::json::array();
		for (auto& in :
		     entry->type().dataOutputs()) {  // outputs to entry are inputs to the function
			inEntry.push_back({{in.name, in.type.qualifiedName()}});
		}

		res.addEntry("EUKN", "Inputs to function doesn't match function inputs",
		             {{"Function Inputs", inFunc}, {"Entry Inputs", inEntry}});
		return res;
	}

	// make sure that the entry node has the functiontype
	if (!std::equal(func.dataOutputs().begin(), func.dataOutputs().end(),
	                func.dataOutputs().begin())) {
		nlohmann::json outFunc = nlohmann::json::array();
		for (auto& out : func.dataOutputs()) {
			outFunc.push_back({{out.name, out.type.qualifiedName()}});
		}

		nlohmann::json outEntry = nlohmann::json::array();
		for (auto& out : func.dataOutputs()) {
			// inputs to the exit are outputs to the function
			outEntry.push_back({{out.name, out.type.qualifiedName()}});
		}

		res.addEntry("EUKN", "Outputs to function doesn't match function exit",
		             {{"Function Outputs", outFunc}, {"Entry Outputs", outEntry}});
		return res;
	}

	auto debugFile = debugBuilder.createFile(debugCU->getFilename(), debugCU->getDirectory());

	// create function type
	llvm::DISubroutineType* subroutineType;
	{
		// create param list
		std::vector<llvm::Metadata*> params;
		{
			// ret first
			DataType intType;
			res += func.context().typeFromModule("lang", "i32", &intType);
			if (!res) { return res; }
			Expects(intType.valid());
			params.push_back(intType.debugType());

			// then first in inputexec id
			params.push_back(intType.debugType());

			// add paramters
			for (const auto& dType : boost::range::join(func.dataInputs(), func.dataOutputs())) {
				params.push_back(dType.type.debugType());
			}
		}

		// create type
		subroutineType = debugBuilder.createSubroutineType(
#if LLVM_VERSION_MAJOR <= 3 && LLVM_VERSION_MINOR <= 7
		    debugFile,
#endif
		    debugBuilder.getOrCreateTypeArray(params));
	}

	auto            mangledName = mangleFunctionName(func.module().fullName(), func.name());
	llvm::Function* f =
	    llvm::cast<llvm::Function>(mod->getOrInsertFunction(mangledName, func.functionType()));

	// TODO(#65): line numbers?
	auto debugFunc = debugBuilder.createFunction(
	    debugFile, func.module().fullName() + ":" + func.name(), mangledName, debugFile, 0,
	    subroutineType, false, true, 0, llvm::DINode::DIFlags{}, false
#if LLVM_VERSION_MAJOR <= 3 && LLVM_VERSION_MINOR <= 7
	    ,
	    f);
#else
	    );

	f->setSubprogram(debugFunc);
#endif
	llvm::BasicBlock* allocBlock = llvm::BasicBlock::Create(mod->getContext(), "alloc", f);
	llvm::BasicBlock* block      = llvm::BasicBlock::Create(mod->getContext(), entry->id(), f);
	auto              blockcpy   = block;

	// set argument names
	auto idx = 0ull;
	for (auto& arg : f->getArgumentList()) {
		// the first one is the input exec ID
		if (idx == 0) {
			arg.setName("inputexec_id");

			// create debug info
			DataType intDataType;
			res += func.context().typeFromModule("lang", "i32", &intDataType);
			Expects(intDataType.valid());
			auto debugParam =
			    debugBuilder.createParameterVariable(debugFunc, "inputexec_id", 1, debugFile, 0,
			                            intDataType.debugType());
			debugBuilder.insertDeclare(&arg, debugParam, debugBuilder.createExpression(),
			                           llvm::DebugLoc::get(1, 1, debugFunc),
			                           allocBlock);  // TODO(#65): "line" numbers

			++idx;
			continue;
		}

		NamedDataType tyAndName;
		// all the - 1's is becaues the first is the inputexec_id
		if (idx - 1 < func.dataInputs().size()) {
			tyAndName = func.dataInputs()[idx - 1];
		} else {
			tyAndName = func.dataOutputs()[idx - 1 - entry->type().dataOutputs().size()];
		}
		arg.setName(tyAndName.name);

		// create debug info

		// create DIType*
		llvm::DIType* dType      = tyAndName.type.debugType();
		auto          debugParam = debugBuilder.createParameterVariable(debugFunc, tyAndName.name,
		                                          idx + 1,  // + 1 because it starts at 1
		                                          debugFile, 0, dType);
		
		debugBuilder.insertDeclare(&arg, debugParam, debugBuilder.createExpression(),
		                           llvm::DebugLoc::get(1, 1, debugFunc),
		                           allocBlock);  // TODO(#65): line numbers

		++idx;
	}

	auto            nodeLocations = func.module().createLineNumberAssoc();
	codegenMetadata codeMetadata{allocBlock, &debugBuilder, debugFunc,
	                             std::unordered_map<NodeInstance*, Cache>{}, nodeLocations};

	codegenHelper(entry, 0, block, codeMetadata, res);

	llvm::IRBuilder<> allocbuilder(allocBlock);
	allocbuilder.CreateBr(blockcpy);

	return res;
}
}  // namespace chig
