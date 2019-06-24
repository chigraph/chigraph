/// \file NodeCompiler.hpp

#pragma once

#ifndef CHI_NODE_COMPILER_HPP
#define CHI_NODE_COMPILER_HPP

#include "chi/Fwd.hpp"

#include <cassert>
#include <unordered_set>
#include <vector>

#include <boost/dynamic_bitset.hpp>

namespace chi {

/// Helper to compile nodes
///
/// Nodes are compiled using many `BasicBlock`s
/// The first are for the dependent pures, one for each.
/// The idea is that you point the last pure to jump to the next pure,
/// and when you get to the last pure, then go to the actual block for this node.
struct NodeCompiler {
	/// Constructor
	/// \param functionCompiler The function compiler instance
	/// \param inst The node to compile
	NodeCompiler(FunctionCompiler& functionCompiler, NodeInstance& inst);

	NodeCompiler(const NodeCompiler&) = delete;

	/// Move const
	NodeCompiler(NodeCompiler&&) = default;

	NodeCompiler& operator=(const NodeCompiler&) = delete;

	/// Move assign
	NodeCompiler& operator=(NodeCompiler&&) = default;

	/// Get the function compiler
	/// \return the FunctionCompiler
	FunctionCompiler& funcCompiler() const { return *mCompiler; }

	/// The node we're compiling
	/// \return The NodeInstance
	NodeInstance& node() const { return *mNode; }

	/// `node().type().pure()`
	/// \return `node().type().pure()`
	bool pure() const;

	/// Add the basic blocks and fill the pure blocks, but don't fill the code block
	/// nop if its already been called with this inputExecID
	/// \param inputExecID The input exec to compile
	/// \pre `inputExecID < inputExecs()`
	void compile_stage1(size_t inputExecID);

	/// Fill the codegen block
	/// If compile_stage1 hasn't been called for this inputExecID, then it will be called
	/// nop if this inputExecID has been compiled before
	/// \param trailingBlocks The basic blocks to br to when the node is done, one for each exec
	/// output
	/// \pre `pure() || (trailingBlock.size() == node().outputExecConnections.size())`
	/// Either it's pure (in which case `trailingBlock` is ignored) or it must be defined
	/// \param inputExecID The input exec ID to compile
	/// \pre `inputExecID < inputExecs()`
	Result compile_stage2(std::vector<LLVMBasicBlockRef> trailingBlocks, size_t inputExecID);

	/// Get if compile_stage2 has been called for a given inputExecID
	/// \param inputExecID the ID to check
	/// \pre `inputExecID < inputExecs()`
	/// \return true if it's already been called, false otherwise
	bool compiled(size_t inputExecID) const;

	/// Get the first block to jump to for the node
	/// If there are dependent pures, it's the first pure block
	/// Otherwise, it's the code block
	/// `compile_stage1` needs to be called first for this ID
	/// \param inputExecID the ID to get the block for.
	/// \pre `inputExecID < inputExecs()`
	/// \return The basic block.
	LLVMBasicBlockRef firstBlock(size_t inputExecID) const;

	/// Get the code block for a given inputExecID
	/// Requires that `compile_stage1` has been called for this ID
	/// \param inputExecID the ID to get the code block for
	/// \pre `inputExecID < inputExecs()`
	/// \return The BasicBlock
	LLVMBasicBlockRef codeBlock(size_t inputExecID) const;

	/// \copydoc chi::FunctionCompiler::llvmModule
	LLVMModuleRef llvmModule() const;

	/// Just node().context()
	Context& context() const;

	/// The number of input execs that we can compile
	/// If it's pure or an entry node, this is 1, otherwise it's
	/// node().inputExecConnections().size()
	/// \return The count
	size_t inputExecs() const;

	/// Get return values
	/// \return a vector of the return values
	std::vector<LLVMValueRef> returnValues() const { return mReturnValues; }

	/// Get the IndirectBrInst* for the pure
	/// \pre `pure()`
	/// \return The indirectbr instance
	LLVMValueRef jumpBackInst() const {
		assert(pure() && "Cannot get jump back inst for a nonpure node");
		return mJumpBackInst;
	}

private:
	FunctionCompiler* mCompiler;
	NodeInstance*     mNode;

	std::vector<std::vector<LLVMBasicBlockRef>> mPureBlocks;
	std::vector<LLVMBasicBlockRef>              mCodeBlocks;

	std::vector<LLVMValueRef> mReturnValues;

	boost::dynamic_bitset<> mCompiledInputs;

	LLVMValueRef mJumpBackInst = nullptr;
};

/// Get the pures a NodeInstance relies on
/// These are all the dependent pures (it's fetched recursively)
/// They are in the order of dependency, for examply if node X depends on node Y, then node Y will
/// come before node X
/// \warning Recursive based on how many pures are in a chain. Cyclic pure dependencies will crash.
/// TODO: make sure they don't crash, and issue a nice error
/// \param inst The NodeInstance to get the dependent pures for
/// \return All the directly dependent pures
/// \post all the elements in the return are pure
std::vector<NodeInstance*> dependentPuresRecursive(const NodeInstance& inst);

}  // namespace chi

#endif  // CHI_NODE_COMPILER_HPP
