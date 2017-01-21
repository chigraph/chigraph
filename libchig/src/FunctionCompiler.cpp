#include "chig/FunctionCompiler.hpp"
#include "chig/NodeInstance.hpp"
#include "chig/FunctionValidator.hpp"
#include "chig/GraphFunction.hpp"
#include "chig/JsonModule.hpp"
#include "chig/NameMangler.hpp"

#include <boost/bimap.hpp>
#include <boost/dynamic_bitset.hpp>
#include <boost/range/join.hpp>

#include <unordered_map>

namespace chig {


struct cache {
	std::vector<llvm::Value*>	  outputs;
	std::vector<llvm::BasicBlock*> inputBlock;  // one for each exec input
};

/// \internal
/// Codegens a single input to a node
void codegenHelper(NodeInstance* node, unsigned execInputID, llvm::BasicBlock* block,
				   llvm::BasicBlock* allocblock, llvm::Module* mod, llvm::DIBuilder* dbuilder,
				   llvm::Function* f, llvm::DISubprogram*	diFunc,
				   std::unordered_map<NodeInstance*, cache>& nodeCache, Result& res,
				   boost::bimap<unsigned, NodeInstance*>& nodeLocations) {
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
							 {{"nodeid", node->id()},
							  {"input ID", inputID},
							  {"nodetype", node->type().qualifiedName()}});

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
							 {{"nodeid", node->id()},
							  {"input ID", inputID},
							  {"nodetype", node->type().qualifiedName()}});

				return;
			}

			// get pointers to the objects
			auto value = cacheObject.outputs[param.second];
			// dereference
			io.push_back(builder.CreateLoad(value));  // TODO: pass ptr to value

			// make sure it's the right type
			if (io[io.size() - 1]->getType() !=
				node->type().dataInputs()[inputID].first.llvmType()) {
				res.addEntry(
					"EINT", "Internal codegen error: unexpected type in cache.",
					{{"Expected LLVM type",
					  stringifyLLVMType(node->type().dataInputs()[inputID].first.llvmType())},
					 {"Found type", stringifyLLVMType(io[io.size() - 1]->getType())},
					 {"Node ID", node->id()},
					 {"Input ID", inputID}});
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
			llvm::AllocaInst* alloc = allocBuilder.CreateAlloca(output.first.llvmType(), nullptr,
																node->id() + "__" + output.second);
			outputCache.push_back(alloc);
			io.push_back(alloc);

			// create debug info
			{
				// get type
				llvm::DIType* dType =
					output.first.module().debugTypeFromName(output.first.unqualifiedName());

				// TODO: better names
                auto debugVar = dbuilder->
#if LLVM_VERSION_MAJOR <= 3 && LLVM_VERSION_MINOR <= 7
                    createLocalVariable(llvm::dwarf::DW_TAG_auto_variable,
#else
                    createAutoVariable(
#endif
                      diFunc, node->id() + "__" + output.second, diFunc->getFile(), 1, dType);

				dbuilder->insertDeclare(alloc, debugVar, dbuilder->createExpression(),
										llvm::DebugLoc::get(1, 1, diFunc), allocblock);
			}

			// make sure the type is right
			if (llvm::PointerType::get(output.first.llvmType(), 0) != alloc->getType()) {
				res.addEntry(
					"EINT", "Internal codegen error: unexpected type returned from alloca.",
					{{"Expected LLVM type",
					  stringifyLLVMType(llvm::PointerType::get(output.first.llvmType(), 0))},
					 {"Yielded type", stringifyLLVMType(alloc->getType())},
					 {"Node ID", node->id()}});
			}
		}
	}

	// add this block to the cache
	if (node->type().qualifiedName() != "lang:entry") {
		nodeCache[node].inputBlock.resize(node->inputExecConnections.size(), nullptr);
		nodeCache[node].inputBlock[execInputID] = block;
	}

	// create output blocks
	std::vector<llvm::BasicBlock*> outputBlocks;
	std::vector<llvm::BasicBlock*> unusedBlocks;
	boost::dynamic_bitset<>
		needsCodegen;  // this holds if each block needs to recurse of it it's already in the cache
	for (auto idx = 0ull; idx < node->outputExecConnections.size(); ++idx) {
		// see if we can find it in the cache
		NodeInstance* remoteNode;
		size_t		  remotePortIdx;
		std::tie(remoteNode, remotePortIdx) = node->outputExecConnections[idx];

		auto remoteCache = nodeCache[remoteNode];  // this will construct it if it doesn't exist
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

	// codegen
	res += node->type().codegen(execInputID, mod,
								llvm::DebugLoc::get(nodeLocations.right.at(node), 1, diFunc), f, io,
								block, outputBlocks);
	if (!res) { return; }

	// TODO: sequence nodes
	for (auto& bb : unusedBlocks) {
		llvm::IRBuilder<> builder(bb);

		builder.CreateRet(
			llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(mod->getContext()), 0, true));
	}

	// recurse!
	for (auto idx = 0ull; idx < node->outputExecConnections.size(); ++idx) {
		auto& output = node->outputExecConnections[idx];
		if (output.first != nullptr && needsCodegen[idx]) {
			codegenHelper(output.first, output.second, outputBlocks[idx], allocblock, mod, dbuilder,
						  f, diFunc, nodeCache, res, nodeLocations);
		}
	}
}


Result compileFunction(const GraphFunction& func, llvm::Module* mod, llvm::DICompileUnit* debugCU,
							  llvm::DIBuilder& debugBuilder) {
	Expects(mod != nullptr);

	Result res;

	res += validateFunction(func);
	
	if(!res) {
		return res;
	}
	
	auto entry = func.entryNode();
	if (entry == nullptr) {
		res.addEntry("EUKN", "No entry node", {});
		return res;
	}

	// make sure that the entry node has the functiontype
	if (!std::equal(func.dataInputs().begin(), func.dataInputs().end(),
					entry->type().dataOutputs().begin())) {
		nlohmann::json inFunc = nlohmann::json::array();
		for (auto& in : func.dataInputs()) { inFunc.push_back({{in.second, in.first.qualifiedName()}}); }

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
	if (!std::equal(func.dataOutputs().begin(), func.dataOutputs().end(), func.dataOutputs().begin())) {
		nlohmann::json outFunc = nlohmann::json::array();
		for (auto& out : func.dataOutputs()) {
			outFunc.push_back({{out.second, out.first.qualifiedName()}});
		}

		nlohmann::json outEntry = nlohmann::json::array();
		for (auto& out : func.dataOutputs()) {
			// inputs to the exit are outputs to the function
			outEntry.push_back({{out.second, out.first.qualifiedName()}});
		}

		res.addEntry("EUKN", "Outputs to function doesn't match function exit",
					 {{"Function Outputs", outFunc}, {"Entry Outputs", outEntry}});
		return res;
	}

	auto debugFile   = debugBuilder.createFile(debugCU->getFilename(), debugCU->getDirectory());
    
	// create function type
	llvm::DISubroutineType* subroutineType;
	{
		// create param list
		std::vector<llvm::Metadata*> params;
		{
			// ret first
			llvm::DIType* intType;
			res += func.context().debugTypeFromModule("lang", "i32", &intType);
			params.push_back(intType);

			// then first in inputexec id
			params.push_back(intType);

			// add paramters
			for (const auto& dType : boost::range::join(func.dataInputs(), func.dataOutputs())) {
				llvm::DIType* debugTy;
				res += func.context().debugTypeFromModule(dType.first.module().name(),
													 dType.first.unqualifiedName(), &debugTy);
				params.push_back(debugTy);
			}
		}

		// create type
		subroutineType =
			debugBuilder.createSubroutineType(
#if LLVM_VERSION_MAJOR <= 3 && LLVM_VERSION_MINOR <= 7
              debugFile,
#endif
              debugBuilder.getOrCreateTypeArray(params));
	}

	auto mangledName = mangleFunctionName(func.module().fullName(), func.name());
	llvm::Function* f =
		llvm::cast<llvm::Function>(mod->getOrInsertFunction(mangledName, func.functionType()));
    
	// TODO: line numbers?
	auto debugFunc =
		debugBuilder.createFunction(debugFile, func.module().fullName() + ":" + func.name(), mangledName,
									debugFile, 0, subroutineType, false, true, 0, 0, false
#if LLVM_VERSION_MAJOR <= 3 && LLVM_VERSION_MINOR <= 7
        , f);
#else
        );

	f->setSubprogram(debugFunc);
#endif
	llvm::BasicBlock* allocblock = llvm::BasicBlock::Create(mod->getContext(), "alloc", f);
	llvm::BasicBlock* block	= llvm::BasicBlock::Create(mod->getContext(), func.name() + "_entry", f);
	auto			  blockcpy = block;

	// follow "linked list"
	unsigned execInputID =
		entry->outputExecConnections[0].second;  // the exec connection to codegen from

	std::unordered_map<NodeInstance*, cache> nodeCache;

	// set argument names
	auto idx = 0ull;
	for (auto& arg : f->getArgumentList()) {
		// the first one is the input exec ID
		if (idx == 0) {
			arg.setName("inputexec_id");

			// create debug info
			llvm::DIType* intDebugType;
			res += func.context().debugTypeFromModule("lang", "i32", &intDebugType);
			auto debugParam = debugBuilder.
#if LLVM_VERSION_MAJOR <= 3 && LLVM_VERSION_MINOR <= 7
              createLocalVariable(llvm::dwarf::DW_TAG_arg_variable, debugFunc, "inputexec_id", debugFile, 0, intDebugType);
#else
              createParameterVariable(debugFunc, "inputexec_id", 1, debugFile, 0, intDebugType);
#endif         
			debugBuilder.insertDeclare(&arg, debugParam, debugBuilder.createExpression(),
									   llvm::DebugLoc::get(1, 1, debugFunc),
									   allocblock);  // TODO: "line" numbers

			++idx;
			continue;
		}

		std::pair<DataType, std::string> tyAndName;
		// all the - 1's is becaues the first is the inputexec_id
		if (idx - 1 < func.dataInputs().size()) {
			tyAndName = func.dataInputs()[idx - 1];
		} else {
			tyAndName = func.dataOutputs()[idx - 1 - entry->type().dataOutputs().size()];
		}
		arg.setName(tyAndName.second);

		// create debug info

		// create DIType*
		llvm::DIType* dType =
			tyAndName.first.module().debugTypeFromName(tyAndName.first.unqualifiedName());
		auto debugParam = debugBuilder.
#if LLVM_VERSION_MAJOR <= 3 && LLVM_VERSION_MINOR <= 7
              createLocalVariable(llvm::dwarf::DW_TAG_arg_variable, debugFunc, tyAndName.second, debugFile, 0, dType);
#else
		createParameterVariable(debugFunc, tyAndName.second, idx + 1, // + 1 because it starts at 1
															   debugFile, 0, dType);
#endif     
		debugBuilder.insertDeclare(&arg, debugParam, debugBuilder.createExpression(),
								   llvm::DebugLoc::get(1, 1, debugFunc),
								   allocblock);  // TODO: line numbers

		++idx;
	}

	auto nodeLocations = func.module().createLineNumberAssoc();
	codegenHelper(entry, execInputID, block, allocblock, mod, &debugBuilder, f, debugFunc,
				  nodeCache, res, nodeLocations);

	llvm::IRBuilder<> allocbuilder(allocblock);
	allocbuilder.CreateBr(blockcpy);

	return res;
}

};
