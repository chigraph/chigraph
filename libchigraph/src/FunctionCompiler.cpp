/// \file FunctionCompiler.cpp

#include "chi/FunctionCompiler.hpp"
#include "chi/Context.hpp"
#include "chi/DataType.hpp"
#include "chi/FunctionValidator.hpp"
#include "chi/GraphFunction.hpp"
#include "chi/GraphModule.hpp"
#include "chi/LLVMVersion.hpp"
#include "chi/NameMangler.hpp"
#include "chi/NodeInstance.hpp"
#include "chi/NodeType.hpp"
#include "chi/LangModule.hpp"
#include "chi/Result.hpp"

#include <boost/bimap.hpp>
#include <boost/dynamic_bitset.hpp>
#include <boost/range/join.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <unordered_map>

#include <llvm/IR/DIBuilder.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>

namespace fs = boost::filesystem;

namespace chi {


FunctionCompiler::FunctionCompiler(const chi::GraphFunction& func, llvm::Module& moduleToGenInto, llvm::DICompileUnit& debugCU, llvm::DIBuilder& debugBuilder)
 :  mModule{&moduleToGenInto}, mDIBuilder{&debugBuilder}, mDebugCU{&debugCU}, mFunction{&func} {}


Result FunctionCompiler::initialize(bool validate) {
	assert(initialized() == false && "Cannot initialize a FunctionCompiler more than once");
	
	Result res;
	auto compilerCtx = res.addScopedContext({{"Function", function().name()}, {"Module", function().module().fullName()}});
	
	if (validate) {
		res += validateFunction(function());
		if (!res) { return res; }
	}
	

	// get the entry node
	auto entry = function().entryNode();
	if (entry == nullptr) {
		res.addEntry("EUKN", "No entry node", {});
		return res;
	}
	
	
	// create function
	auto mangledName = mangleFunctionName(module().fullName(), function().name());
	mLLFunction =
	    llvm::cast<llvm::Function>(llModule().getOrInsertFunction(mangledName, function().functionType()));

	// create the debug file
	auto debugFile = diBuilder().createFile(debugCompileUnit().getFilename(), debugCompileUnit().getDirectory());

	auto subroutineType = createSubroutineType();
		
	mNodeLocations = module().createLineNumberAssoc();
	auto entryLN       = nodeLineNumber(*entry);

	// TODO(#65): line numbers?
	mDebugFunc =
	    diBuilder().createFunction(debugFile, module().fullName() + ":" + function().name(),
	                                mangledName, debugFile, entryLN, subroutineType, false, true, 0,
#if LLVM_VERSION_LESS_EQUAL(3, 6)
	                                0,
#else
	                                llvm::DINode::DIFlags{},
#endif
	                                false
#if LLVM_VERSION_LESS_EQUAL(3, 7)
	                                , mLLFunction);
#else
	                                );
#	if LLVM_VERSION_LESS_EQUAL(3, 9)
	f->setSubprogram(mDebugFunc);
#	endif
#endif

	mAllocBlock = llvm::BasicBlock::Create(context().llvmContext(), "alloc", mLLFunction);
	llvm::BasicBlock* entryBlock =
	    llvm::BasicBlock::Create(context().llvmContext(), boost::uuids::to_string(entry->id()), mLLFunction);

	// set argument names
	auto idx = 0ull;
	for (auto& arg : mLLFunction->
#if LLVM_VERSION_AT_LEAST(5, 0)
			args()
#else
			getArgumentList()
#endif	
	) {
		// the first one is the input exec ID
		if (idx == 0) {
			arg.setName("inputexec_id");

			// create debug info
			DataType intDataType;
			res += context().typeFromModule("lang", "i32", &intDataType);
			assert(intDataType.valid());
			auto debugParam = diBuilder().
#if LLVM_VERSION_LESS_EQUAL(3, 7)
			                  createLocalVariable(llvm::dwarf::DW_TAG_arg_variable, mDebugFunc,
			                                      "inputexec_id", debugFile, entryLN,
#if LLVM_VERSION_LESS_EQUAL(3, 6)
			                                      *
#endif
			                                      intDataType.debugType());
#else

			                  createParameterVariable(mDebugFunc, "inputexec_id", 1, debugFile,
			                                          entryLN, intDataType.debugType());
#endif
			diBuilder()
			    .insertDeclare(&arg, debugParam,
#if LLVM_VERSION_AT_LEAST(3, 6)
			                   diBuilder().createExpression(),
#	if LLVM_VERSION_AT_LEAST(3, 7)
			                   llvm::DebugLoc::get(entryLN, 1, mDebugFunc),
#	endif
#endif
			                   &allocBlock())
#if LLVM_VERSION_LESS_EQUAL(3, 6)
			    ->setDebugLoc(llvm::DebugLoc::get(entryLN, 1, mDebugFunc))
#endif
			    ;  // TODO(#65): "line" numbers

			++idx;
			continue;
		}

		NamedDataType tyAndName;
		// all the - 1's is becaues the first is the inputexec_id
		if (idx - 1 < function().dataInputs().size()) {
			tyAndName = function().dataInputs()[idx - 1];
		} else {
			tyAndName = function().dataOutputs()[idx - 1 - entry->type().dataOutputs().size()];
		}
		arg.setName(tyAndName.name);

		// create debug info

		// create DIType*
		llvm::DIType* dType      = tyAndName.type.debugType();
		auto          debugParam = diBuilder().
#if LLVM_VERSION_LESS_EQUAL(3, 7)
		                  createLocalVariable(llvm::dwarf::DW_TAG_arg_variable, mDebugFunc,
		                                      tyAndName.name, debugFile, entryLN,
#if LLVM_VERSION_LESS_EQUAL(3, 6)
		                                      *
#endif
		                                      dType);
#else
		                  createParameterVariable(mDebugFunc, tyAndName.name,
		                                          idx + 1,  // + 1 because it starts at 1
		                                          debugFile, entryLN, dType);
#endif
		diBuilder()
		    .insertDeclare(&arg, debugParam,
#if LLVM_VERSION_AT_LEAST(3, 6)
		                   diBuilder().createExpression(),
#	if LLVM_VERSION_AT_LEAST(3, 7)
		                   llvm::DebugLoc::get(entryLN, 1, mDebugFunc),
#	endif
#endif
		                   &allocBlock())
#if LLVM_VERSION_LESS_EQUAL(3, 6)
		    ->setDebugLoc(llvm::DebugLoc::get(entryLN, 1, mDebugFunc))
#endif
		    ;  // TODO(#65): line numbers

		++idx;
	}
	
	mInitialized = true;

	// create mPostPureBreak
	llvm::IRBuilder<> allocBuilder{&allocBlock()};
	mPostPureBreak = allocBuilder.CreateAlloca(llvm::IntegerType::getInt8PtrTy(context().llvmContext()));
	
	return res;
}

Result FunctionCompiler::compilePureDependencies(NodeInstance& node) {
	Result res;
	
	auto depPures = dependentPuresRecursive(node);
	for (auto pure : depPures) {
		res += compileNode(*pure, 0);
		
		if (!res) { return res; }
	}
	return res;
}

Result FunctionCompiler::compileNode(NodeInstance& node, size_t inputExecID) {
	
	Result res;
	
	if (node.type().pure()) {
		// just compile it
		auto compiler = getOrCreateNodeCompiler(node);
		res += compiler->compile_stage2({}, inputExecID);
		
		return res;
	}
	
	// if it's not pure
	
	// compile the dependent pures
	res += compilePureDependencies(node);
	if (!res ) { return res; }

	std::vector<llvm::BasicBlock*> outputBlocks;
	// make sure the output nodes have done stage 1 and collect output blocks
	for (const auto& conn : node.outputExecConnections) {
		res += compilePureDependencies(*conn.first);
		if (!res) { return res; }
		
		auto compiler = getOrCreateNodeCompiler(*conn.first);
		compiler->compile_stage1(conn.second);
		
		outputBlocks.push_back(&compiler->firstBlock(conn.second));
	}
	
	// compile this one
	auto compiler = getOrCreateNodeCompiler(node);
	res += compiler->compile_stage2(outputBlocks, inputExecID);
	if (!res) {
		return res;
	}
	
	// recurse
	for (const auto& conn : node.outputExecConnections) {
		res += compileNode(*conn.first, conn.second);
		if (!res ) {
			return res;
		}
	}
	
	return res;
}


Result FunctionCompiler::compile() {
	
	assert(initialized() && "You must initialize a FunctionCompiler before you compile it");
	assert(compiled() == false && "You cannot compile a FunctionCompiler twice");
	
	// compile the entry
	auto entry = function().entryNode();
	assert(entry != nullptr);
	
	// recurse
	return compileNode(*entry, 0);
}

llvm::DISubroutineType* FunctionCompiler::createSubroutineType() {

	// create param list
	std::vector<
#if LLVM_VERSION_LESS_EQUAL(3, 5)
	    llvm::Value*
#else
	    llvm::Metadata*
#endif
	    > params;
	{
		// ret first
		DataType intType = function().context().langModule()->typeFromName("i32");
		assert(intType.valid());
		
		params.push_back(
#if LLVM_VERSION_LESS_EQUAL(3, 6)
		    *
#endif
		    intType.debugType());

		// then first in inputexec id
		params.push_back(
#if LLVM_VERSION_LESS_EQUAL(3, 6)
		    *
#endif
		    intType.debugType());

		// add paramters
		for (const auto& dType : boost::range::join(function().dataInputs(), function().dataOutputs())) {
			params.push_back(
#if LLVM_VERSION_LESS_EQUAL(3, 6)
			    *
#endif
			    dType.type.debugType());
		}
	}

	// create type
	auto subroutineType = diBuilder().createSubroutineType(
#if LLVM_VERSION_LESS_EQUAL(3, 7)
	    debugFile,
#endif
	    diBuilder().
#if LLVM_VERSION_LESS_EQUAL(3, 5)
			getOrCreateArray
#else
			getOrCreateTypeArray
#endif
				(params));
	
	return subroutineType;
}

GraphModule& FunctionCompiler::module() const {
    return function().module();
}
Context& FunctionCompiler::context() const {
    return function().context();
}

int FunctionCompiler::nodeLineNumber(NodeInstance& node) {
	auto iter = mNodeLocations.right.find(&node);
	if (iter == mNodeLocations.right.end()) {
		return -1; // ?
	}
	return iter->second;
	
}

NodeCompiler* FunctionCompiler::nodeCompiler(NodeInstance& node) {
	assert(&node.function() == &function() && "Cannot get a NodeCompiler for a node instance not in this function");
	
	auto iter = mNodeCompilers.find(&node);
	if (iter != mNodeCompilers.end()) {
		return &iter->second;
	}
	return nullptr;
}

NodeCompiler* FunctionCompiler::getOrCreateNodeCompiler(NodeInstance& node) {
	assert(&node.function() == &function() && "Cannot get a NodeCompiler for a node instance not in this function");
	
	auto iter = mNodeCompilers.find(&node);
	if (iter != mNodeCompilers.end()) {
		return &iter->second;
	}
	return &mNodeCompilers.emplace(&node, NodeCompiler{*this, node}).first->second;
	
}

Result compileFunction(const GraphFunction& func, llvm::Module* mod, llvm::DICompileUnit* debugCU,
                       llvm::DIBuilder& debugBuilder) {
	
	FunctionCompiler compiler{func, *mod, *debugCU, debugBuilder};
	
	auto res = compiler.initialize();
	if (!res) { return res; }
	
	res += compiler.compile();
	
	return res;
}

}  // namespace chi
