/// \file FunctionCompiler.cpp

#include "chi/FunctionCompiler.hpp"

#include <llvm-c/DebugInfo.h>

#include <boost/bimap.hpp>
#include <boost/dynamic_bitset.hpp>
#include <boost/range/join.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <unordered_map>

#include "chi/Context.hpp"
#include "chi/DataType.hpp"
#include "chi/FunctionValidator.hpp"
#include "chi/GraphFunction.hpp"
#include "chi/GraphModule.hpp"
#include "chi/LangModule.hpp"
#include "chi/NameMangler.hpp"
#include "chi/NodeInstance.hpp"
#include "chi/NodeType.hpp"
#include "chi/Support/Result.hpp"

namespace fs = std::filesystem;

namespace chi {

FunctionCompiler::FunctionCompiler(const GraphFunction& func, LLVMModuleRef moduleToGenInto,
                                   LLVMMetadataRef debugFile, LLVMMetadataRef debugCU,
                                   LLVMDIBuilderRef debugBuilder)
    : mModule{moduleToGenInto},
      mDIBuilder{debugBuilder},
      mDIFile{debugFile},
      mDebugCU{debugCU},
      mFunction{&func} {}

Result FunctionCompiler::initialize(bool validate) {
	assert(initialized() == false && "Cannot initialize a FunctionCompiler more than once");

	mInitialized = true;

	Result res;
	auto   compilerCtx = res.addScopedContext(
        {{"Function", function().name()}, {"Module", function().module().fullName()}});

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
	mLLFunction      = LLVMGetNamedFunction(llvmModule(), mangledName.c_str());
	if (mLLFunction == nullptr) {
		mLLFunction = LLVMAddFunction(llvmModule(), mangledName.c_str(),
		                              function().functionType());  // create the debug file
	}

	auto subroutineType = createSubroutineType();

	auto [locationByNode, nodeByLocation] = module().createLineNumberAssoc();
	mNodeByLocation                       = std::move(nodeByLocation);
	mLocationByNode                       = std::move(locationByNode);
	auto entryLN                          = nodeLineNumber(*entry);

	// TODO(#65): line numbers?
	auto name = module().fullName() + ":" + function().name();
	mDebugFunc =
	    LLVMDIBuilderCreateFunction(diBuilder(), mDIFile, name.c_str(), name.length(),
	                                mangledName.c_str(), mangledName.length(), mDIFile, entryLN,
	                                subroutineType, false, true, 0, LLVMDIFlagZero, false);

	LLVMSetSubprogram(mLLFunction, mDebugFunc);

	mAllocBlock = LLVMAppendBasicBlockInContext(context().llvmContext(), mLLFunction, "alloc");

	// set argument names
	auto idx = 0ull;
	for (auto arg = LLVMGetFirstParam(mLLFunction); arg != nullptr; arg = LLVMGetNextParam(arg)) {
		// the first one is the input exec ID
		if (idx == 0) {
			LLVMSetValueName(arg, "inputexec_id");

			// create debug info
			DataType intDataType;
			res += context().typeFromModule("lang", "i32", &intDataType);
			assert(intDataType.valid());

			auto debugParam = LLVMDIBuilderCreateParameterVariable(
			    diBuilder(), mDebugFunc, "inputexec_id", strlen("inputexec_id"), idx, mDIFile,
			    entryLN, intDataType.debugType(*this), false, LLVMDIFlagZero);

			LLVMDIBuilderInsertDeclareAtEnd(
			    diBuilder(), arg, debugParam,
			    LLVMDIBuilderCreateExpression(diBuilder(), nullptr, 0),
			    LLVMDIBuilderCreateDebugLocation(context().llvmContext(), entryLN, 1, mDebugFunc,
			                                     nullptr),
			    allocBlock());

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
		LLVMSetValueName2(arg, tyAndName.name.c_str(), tyAndName.name.size());

		// create DIType*
		LLVMMetadataRef dType      = tyAndName.type.debugType(*this);
		auto            debugParam = LLVMDIBuilderCreateParameterVariable(
            diBuilder(), mDebugFunc, tyAndName.name.c_str(), tyAndName.name.length(), idx + 1,
            mDIFile, entryLN, dType, false, LLVMDIFlagZero);

		LLVMDIBuilderInsertDeclareAtEnd(
		    diBuilder(), arg, debugParam, LLVMDIBuilderCreateExpression(diBuilder(), nullptr, 0),
		    LLVMDIBuilderCreateDebugLocation(context().llvmContext(), entryLN, 1, mDebugFunc,
		                                     nullptr),
		    allocBlock());

		++idx;
	}

	// create mPostPureBreak
	auto allocBuilder = OwnedLLVMBuilder(LLVMCreateBuilderInContext(context().llvmContext()));
	LLVMPositionBuilder(*allocBuilder, allocBlock(), nullptr);

	mPostPureBreak = LLVMBuildAlloca(
	    *allocBuilder, LLVMPointerType(LLVMInt8TypeInContext(context().llvmContext()), 0),
	    "pure_jumpback");

	// alloc local variables and zero them
	for (const auto& localVar : function().localVariables()) {
		mLocalVariables[localVar.name] = LLVMBuildAlloca(*allocBuilder, localVar.type.llvmType(),
		                                                 ("var_" + localVar.name).c_str());
		LLVMBuildStore(*allocBuilder, LLVMConstNull(localVar.type.llvmType()),
		               mLocalVariables[localVar.name]);
	}

	return res;
}

Result FunctionCompiler::compile() {
	assert(initialized() && "You must initialize a FunctionCompiler before you compile it");
	assert(compiled() == false && "You cannot compile a FunctionCompiler twice");

	// compile the entry
	auto entry = function().entryNode();
	assert(entry != nullptr);

	std::deque<std::pair<NodeInstance*, size_t>> nodesToCompile;
	nodesToCompile.emplace_back(entry, 0);

	Result res;

	auto compilePureDependencies = [this](NodeInstance& node) {
		Result res;

		auto depPures = dependentPuresRecursive(node);
		for (auto pure : depPures) {
			auto compiler = getOrCreateNodeCompiler(*pure);
			res += compiler->compile_stage2({}, 0);

			if (!res) { return res; }
		}
		return res;
	};

	while (!nodesToCompile.empty()) {
		auto& node        = *nodesToCompile[0].first;
		auto  inputExecID = nodesToCompile[0].second;

		assert(!node.type().pure());

		auto compiler = getOrCreateNodeCompiler(node);
		if (compiler->compiled(inputExecID)) {
			nodesToCompile.pop_front();

			continue;
		}

		// compile dependent pures
		res += compilePureDependencies(node);
		if (!res) { return res; }

		std::vector<LLVMBasicBlockRef> outputBlocks;
		// make sure the output nodes have done stage 1 and collect output blocks
		for (const auto& conn : node.outputExecConnections) {
			res += compilePureDependencies(*conn.first);
			if (!res) { return res; }

			auto depCompiler = getOrCreateNodeCompiler(*conn.first);
			depCompiler->compile_stage1(conn.second);

			outputBlocks.push_back(depCompiler->firstBlock(conn.second));
		}

		// compile this one
		res += compiler->compile_stage2(outputBlocks, inputExecID);
		if (!res) { return res; }

		// recurse
		for (const auto& conn : node.outputExecConnections) {
			// add them to the end
			nodesToCompile.emplace_back(conn.first, conn.second);
		}

		// pop it off
		nodesToCompile.pop_front();
	}

	if (!res) { return res; }

	auto allocBuilder = OwnedLLVMBuilder(LLVMCreateBuilderInContext(context().llvmContext()));
	LLVMPositionBuilder(*allocBuilder, allocBlock(), nullptr);
	LLVMBuildBr(*allocBuilder, nodeCompiler(*entry)->firstBlock(0));

	return res;
}

LLVMMetadataRef FunctionCompiler::createSubroutineType() {
	// create param list
	std::vector<LLVMMetadataRef> params;
	{
		// ret first
		DataType intType = function().context().langModule()->typeFromName("i32");
		assert(intType.valid());

		params.push_back(intType.debugType(*this));

		// then first in inputexec id
		params.push_back(intType.debugType(*this));

		// add paramters
		for (const auto& dType :
		     boost::range::join(function().dataInputs(), function().dataOutputs())) {
			params.push_back(dType.type.debugType(*this));
		}
	}

	// create type
	return LLVMDIBuilderCreateSubroutineType(diBuilder(), mDIFile, params.data(), params.size(),
	                                         LLVMDIFlagZero);
}

LLVMValueRef FunctionCompiler::localVariable(std::string_view name) {
	assert(initialized() &&
	       "Please initialize the function compiler before getting a local variable");

	auto iter = mLocalVariables.find(std::string(name));
	if (iter != mLocalVariables.end()) { return iter->second; }
	return nullptr;
}

GraphModule& FunctionCompiler::module() const { return function().module(); }
Context&     FunctionCompiler::context() const { return function().context(); }

int FunctionCompiler::nodeLineNumber(NodeInstance& node) {
	assert(&node.function() == &function() &&
	       "Cannot get node line number for a node not in the function");

	auto iter = mLocationByNode.find(&node);
	if (iter == mLocationByNode.end()) {
		return -1;  // ?
	}
	return iter->second;
}

NodeCompiler* FunctionCompiler::nodeCompiler(NodeInstance& node) {
	assert(&node.function() == &function() &&
	       "Cannot get a NodeCompiler for a node instance not in this function");

	auto iter = mNodeCompilers.find(&node);
	if (iter != mNodeCompilers.end()) { return &iter->second; }
	return nullptr;
}

NodeCompiler* FunctionCompiler::getOrCreateNodeCompiler(NodeInstance& node) {
	assert(&node.function() == &function() &&
	       "Cannot get a NodeCompiler for a node instance not in this function");

	auto iter = mNodeCompilers.find(&node);
	if (iter != mNodeCompilers.end()) { return &iter->second; }
	return &mNodeCompilers.emplace(&node, NodeCompiler{*this, node}).first->second;
}

Result compileFunction(const GraphFunction& func, LLVMModuleRef mod, LLVMMetadataRef debugFile,
                       LLVMMetadataRef debugCU, LLVMDIBuilderRef debugBuilder) {
	FunctionCompiler compiler{func, mod, debugFile, debugCU, debugBuilder};

	auto res = compiler.initialize();
	if (!res) { return res; }

	res += compiler.compile();

	return res;
}

}  // namespace chi
