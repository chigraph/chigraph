#include "chi/NodeCompiler.hpp"

#include <llvm-c/Core.h>

#include <cassert>

#include "chi/Context.hpp"
#include "chi/DataType.hpp"
#include "chi/FunctionCompiler.hpp"
#include "chi/NodeInstance.hpp"
#include "chi/NodeType.hpp"
#include "chi/Support/Result.hpp"

namespace fs = std::filesystem;

namespace chi {

NodeCompiler::NodeCompiler(FunctionCompiler& functionCompiler, NodeInstance& inst)
    : mCompiler{&functionCompiler}, mNode{&inst} {
	// alloca the outputs
	auto allocBuilder = OwnedLLVMBuilder(LLVMCreateBuilderInContext(context().llvmContext()));
	LLVMPositionBuilder(*allocBuilder, funcCompiler().allocBlock(), nullptr);

	for (auto idx = 0ull; idx < node().type().dataOutputs().size(); ++idx) {
		const auto& namedType = node().type().dataOutputs()[idx];

		// alloca the outputs
		auto alloca = LLVMBuildAlloca(*allocBuilder, namedType.type.llvmType(),
		                              (node().stringId() + "__" + std::to_string(idx)).c_str());

		// create debug info for the alloca
		{
			// get type
			LLVMMetadataRef dType = namedType.type.debugType(funcCompiler());

			// TODO(#63): better names
			auto name     = node().stringId() + "__" + std::to_string(idx);
			auto debugVar = LLVMDIBuilderCreateAutoVariable(
			    funcCompiler().diBuilder(), funcCompiler().diFunction(), name.c_str(),
			    name.length(), funcCompiler().debugFile(), 1, dType, false, LLVMDIFlagZero, 0);

			LLVMDIBuilderInsertDeclareAtEnd(
			    funcCompiler().diBuilder(), alloca, debugVar,
			    LLVMDIBuilderCreateExpression(funcCompiler().diBuilder(), nullptr, 0),
			    LLVMDIBuilderCreateDebugLocation(context().llvmContext(), 1, 1,
			                                     funcCompiler().diFunction(), nullptr),
			    funcCompiler().allocBlock());
		}

		mReturnValues.push_back(alloca);
	}

	auto size = inputExecs();

	// resize the inputexec specific variables
	mPureBlocks.resize(size);
	mCodeBlocks.resize(size, nullptr);

	mCompiledInputs.resize(size, false);
}

bool NodeCompiler::pure() const { return node().type().pure(); }

void NodeCompiler::compile_stage1(size_t inputExecID) {
	assert(inputExecID < inputExecs() &&
	       "Cannot compile_stage1 for a inputexec that doesn't exist");

	// create the code block
	auto& codeBlock = mCodeBlocks[inputExecID];

	// if we've already done stage 1 before, then just exit
	if (codeBlock != nullptr) { return; }

	codeBlock = LLVMAppendBasicBlockInContext(
	    context().llvmContext(), funcCompiler().llFunction(),
	    ("node_" + node().stringId() + "__" + std::to_string(inputExecID)).c_str());

	// only do this for non-pure nodes because pure nodes don't call their dependencies, they are
	// called by the non-pure
	if (!pure()) {
		// generate code for all the dependent pures
		auto depPures = dependentPuresRecursive(node());

		// set our vector to be the same length
		auto& pureBlocks = mPureBlocks[inputExecID];
		pureBlocks.resize(depPures.size());

		// create the first pure block--the loop only creates the next one
		if (!depPures.empty()) {
			auto name = "node_" + node().stringId() + "__" + std::to_string(inputExecID) + "__" +
			            depPures[0]->stringId();

			pureBlocks[0] = LLVMAppendBasicBlockInContext(
			    context().llvmContext(), funcCompiler().llFunction(), name.c_str());
		}

		for (auto id = 0ull; id < depPures.size(); ++id) {
			// create a BasicBlock for the next one to br to--if we're on the last one, use the code
			// block
			LLVMBasicBlockRef nextBlock = [&] {
				if (id == depPures.size() - 1) { return codeBlock; }
				auto name = "node_" + node().stringId() + "__" + std::to_string(inputExecID) +
				            "__" + depPures[id + 1]->stringId();
				pureBlocks[id + 1] = LLVMAppendBasicBlockInContext(
				    context().llvmContext(), funcCompiler().llFunction(), name.c_str());
				return pureBlocks[id + 1];
			}();

			// add nextBlock to the list of possible locations for the indirectbr
			LLVMAddDestination(funcCompiler().nodeCompiler(*depPures[id])->jumpBackInst(),
			                   nextBlock);

			// set post-pure break to go to the next one
			auto irBuilder = OwnedLLVMBuilder(LLVMCreateBuilderInContext(context().llvmContext()));
			LLVMPositionBuilder(*irBuilder, pureBlocks[id], nullptr);
			LLVMBuildStore(*irBuilder, LLVMBlockAddress(funcCompiler().llFunction(), nextBlock),
			               funcCompiler().postPureBreak());

			// br to the pure, terminating that BasicBlock
			LLVMBuildBr(*irBuilder, funcCompiler().nodeCompiler(*depPures[id])->firstBlock(0));
		}
	}
}

Result NodeCompiler::compile_stage2(std::vector<LLVMBasicBlockRef> trailingBlocks,
                                    size_t                         inputExecID) {
	assert((pure() || trailingBlocks.size() == node().outputExecConnections.size()) &&
	       "Trailing blocks is the wrong size");
	assert(inputExecID < inputExecs());

	auto& codeBlock = mCodeBlocks[inputExecID];

	// skip if we've already compiled
	if (compiled(inputExecID)) { return {}; }

	// if we haven't done stage 1, then do it
	if (codeBlock == nullptr) { compile_stage1(inputExecID); }
	auto codeBuilder = OwnedLLVMBuilder(LLVMCreateBuilderInContext(context().llvmContext()));
	LLVMPositionBuilder(*codeBuilder, codeBlock, nullptr);

	// inputs and outputs (inputs followed by outputs)
	std::vector<LLVMValueRef> io;

	// add inputs
	for (auto idx = 0ull; idx < node().inputDataConnections.size(); ++idx) {
		auto& connection = node().inputDataConnections[idx];
		auto& remoteNode = *connection.first;
		auto  remoteID   = connection.second;

		assert(remoteID < funcCompiler().nodeCompiler(remoteNode)->returnValues().size() &&
		       "Internal error: connection to a value doesn't exist");

		auto loaded = LLVMBuildLoad(
		    *codeBuilder, funcCompiler().nodeCompiler(remoteNode)->returnValues()[remoteID], "");
		io.push_back(loaded);

		assert(LLVMTypeOf(io[io.size() - 1]) == node().type().dataInputs()[idx].type.llvmType() &&
		       "Internal error: types do not match");
	}

	// add outputs
	std::copy(mReturnValues.begin(), mReturnValues.end(), std::back_inserter(io));

	// on pure, jump to the set loc
	if (pure()) {
		auto brBlock =
		    LLVMAppendBasicBlockInContext(context().llvmContext(), funcCompiler().llFunction(),
		                                  ("node_" + node().stringId() + "_jumpback").c_str());
		auto builder = OwnedLLVMBuilder(LLVMCreateBuilderInContext(context().llvmContext()));
		LLVMPositionBuilder(*builder, brBlock, nullptr);

		mJumpBackInst = LLVMBuildIndirectBr(
		    *builder, LLVMBuildLoad(*builder, funcCompiler().postPureBreak(), ""),
		    10);  // 10 is just the default

		trailingBlocks.resize(1);
		trailingBlocks[0] = brBlock;
	}

	// codegen
	Result res =
	    node().type().codegen(*this, codeBlock, inputExecID,
	                          LLVMDIBuilderCreateDebugLocation(
	                              context().llvmContext(), funcCompiler().nodeLineNumber(node()), 1,
	                              funcCompiler().diFunction(), nullptr),
	                          io, trailingBlocks);

	mCompiledInputs[inputExecID] = true;

	return res;
}

LLVMBasicBlockRef NodeCompiler::firstBlock(size_t inputExecID) const {
	assert(inputExecID < inputExecs());

	if (mPureBlocks[inputExecID].empty()) { return codeBlock(inputExecID); }
	return mPureBlocks[inputExecID][0];
}

LLVMBasicBlockRef NodeCompiler::codeBlock(size_t inputExecID) const {
	assert(inputExecID < inputExecs());
	return mCodeBlocks[inputExecID];
}

LLVMModuleRef NodeCompiler::llvmModule() const { return funcCompiler().llvmModule(); }

bool NodeCompiler::compiled(size_t inputExecID) const {
	assert(inputExecID < inputExecs());
	return mCompiledInputs[inputExecID];
}

Context& NodeCompiler::context() const { return node().context(); }

size_t NodeCompiler::inputExecs() const {
	if (pure()) {
		return 1;
	} else if (node().type().qualifiedName() == "lang:entry") {
		return 1;
	}
	return node().inputExecConnections.size();
}

std::vector<NodeInstance*> dependentPuresRecursive(const NodeInstance& inst) {
	std::vector<NodeInstance*> ret;

	// TODO: remove duplicates in some intellegent way
	for (const auto& conn : inst.inputDataConnections) {
		// if it isn't connected (this really shouldn't happen because that would fail validation),
		// then skip.
		if (conn.first == nullptr) { continue; }

		if (conn.first->type().pure()) {
			auto deps = dependentPuresRecursive(*conn.first);
			std::copy(deps.begin(), deps.end(), std::back_inserter(ret));

			ret.push_back(conn.first);
		}
	}

	return ret;
}

}  // namespace chi
