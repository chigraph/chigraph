/// \file NodeCompiler.hpp

#pragma once

#ifndef CHI_NODE_COMPILER_HPP
#define CHI_NODE_COMPILER_HPP

#include "chi/Fwd.hpp"

#include <vector>
#include <unordered_set>
#include <cassert>

#include <boost/dynamic_bitset.hpp>

namespace chi {

/// Helper to compile nodes
///
/// Nodes are compiled using many `BasicBlock`s
/// The first are for the dependent pures, one for each.
/// The idea is that you point the last pure to jump to the next pure,
/// and when you get to the last pure, then go to the actual block for this node.
struct NodeCompiler {
	
	NodeCompiler(FunctionCompiler& functionCompiler, NodeInstance& inst);
	
	NodeCompiler(const NodeCompiler&) = delete;
	NodeCompiler(NodeCompiler&&) = default;
	
	NodeCompiler& operator=(const NodeCompiler&) = delete;
	NodeCompiler& operator=(NodeCompiler&&) = default;
	
	FunctionCompiler& funcCompiler() const { return *mCompiler; }
	NodeInstance& node() const { return *mNode; }
	
	// helper functon for node().type().pure()
	bool pure() const;
	
	/// Add the basic blocks and fill the pure blocks, but don't fill the code block
	void compile_stage1(size_t inputExecID);
	
	/// Fill the codegen block
	/// \param trailingBlocks The basic blocks to br to when the node is done, one for each exec output
	/// \pre `pure() || (trailingBlock != nullptr)` 
	/// Either it's pure (in which case `trailingBlock` is ignored) or it must be defined
	/// \pre `inputExecID < inputExecs()`
	Result compile_stage2(std::vector<llvm::BasicBlock*> trailingBlocks, size_t inputExecID);
	
	/// \pre `inputExecID < inputExecs()`
	bool compiled(size_t inputExecID) const;
	
	/// \pre `inputExecID < inputExecs()`
	llvm::BasicBlock& firstBlock(size_t inputExecID) const;
	
	/// \pre `inputExecID < inputExecs()`
	llvm::BasicBlock& codeBlock(size_t inputExecID) const;
	
	llvm::Module& llvmModule() const;
	
	/// Just node().context()
	Context& context() const;
	
	/// The number of input execs that we can compile
	/// If it's pure or an entry node, this is 1, otherwise it's node().inputExecConnections().size()
	size_t inputExecs() const;
	
	/// Get return values
	std::vector<llvm::Value*> returnValues() const { return mReturnValues; }
	
	/// Get the IndirectBrInst* for the pure (always nullptr if this node is nonpure)
	llvm::IndirectBrInst* jumpBackInst() const { return mJumpBackInst; }
	
private:
	
	FunctionCompiler* mCompiler;
	NodeInstance* mNode;
	
	std::vector<std::vector<llvm::BasicBlock*>> mPureBlocks;
	std::vector<llvm::BasicBlock*> mCodeBlocks;
	
	std::vector<llvm::Value*> mReturnValues;
	
	boost::dynamic_bitset<> mCompiledInputs;
	
	llvm::IndirectBrInst* mJumpBackInst = nullptr;
	
};


/// Get the pures a NodeInstance relies on
/// These are all the dependent pures (it's fetched recursively)
/// They are in the order of dependency, for examply if node X depends on node Y, then node Y will come before node X
/// \warning Recursive based on how many pures are in a chain. Cyclic pure dependencies will crash.
/// TODO: make sure they don't crash, and issue a nice error
/// \param inst The NodeInstance to get the dependent pures for
/// \return All the directly dependent pures
/// \post all the elements in the return are pure
std::vector<NodeInstance*> dependentPuresRecursive(const NodeInstance& inst);

} // namespace chi


#endif // CHI_NODE_COMPILER_HPP
