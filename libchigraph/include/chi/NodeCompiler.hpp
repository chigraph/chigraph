/// \file NodeCompiler.hpp

#pragma once

#ifndef CHI_NODE_COMPILER_HPP
#define CHI_NODE_COMPILER_HPP

#include "chi/Fwd.hpp"

#include <vector>
#include <unordered_set>
#include <cassert>

namespace chi {

/// Helper to compile nodes
///
/// Nodes are compiled using many `BasicBlock`s
/// The first are for the dependent pures, one for each.
/// The idea is that you point the last pure to jump to the next pure,
/// and when you get to the last pure, then go to the actual block for this node.
struct NodeCompiler {
	NodeCompiler(FunctionCompiler& funcCompiler, NodeInstance& inst);
	
	NodeCompiler(const NodeCompiler&) = delete;
	NodeCompiler(NodeCompiler&&) = default;
	
	NodeCompiler& operator=(const NodeCompiler&) = delete;
	NodeCompiler& operator=(NodeCompiler&&) = default;
	
	FunctionCompiler& funcCompiler() const { return *mCompiler; }
	NodeInstance& node() const { return *mNode; }
	
	bool compiled() const {
		return mCompiled;
	}
	
	// helper functon for node().type().pure()
	bool pure() const;
	
	/// Add the basic blocks to the function
	/// \param trailingBlock The basic blocks to br to when the node is done, one for each exec output
	/// \pre `pure() || (trailingBlock != nullptr)` 
	/// Either it's pure (in which case `trailingBlock` is ignored) or it must be defined
	void compile(const std::vector<llvm::BasicBlock>& trailingBlocks);
	
	/// \pre `compiled()`
	llvm::BasicBlock& firstBlock() const;
	
	/// \pre `compiled()`
	llvm::BasicBlock& codeBlock() const { assert(compiled() && "Cannot get code block for node before compiling it"); return *mCodeBlock; }
	
	/// Just node().context()
	Context& context() const;
	
	/// Get return values
	/// \pre `compiled() == true`
	std::vector<llvm::Value*> returnValues() const { assert(compiled() && "Cannot get return values before compiling"); return mReturnValues; }
	
private:
	
	bool mCompiled = false;
	
	FunctionCompiler* mCompiler;
	NodeInstance* mNode;
	
	std::vector<llvm::BasicBlock*> mPureBlocks;
	llvm::BasicBlock* mCodeBlock = nullptr;
	
	std::vector<llvm::Value*> mReturnValues;
	
};


/// Get the pures a NodeInstance relies on
/// These are all the dependent pures (it's fetched recursively)
/// They are in the order of dependency, for examply if node X depends on node Y, then node Y will come before node X
/// \note Recursive based on how many pures are in a chain. Cyclic pure dependencies will crash.
/// TODO: make sure they don't crash, and issue a nice error
/// \param inst The NodeInstance to get the dependent pures for
/// \return All the directly dependent pures
/// \post all the elements in the return are pure
std::vector<NodeInstance*> dependentPuresRecursive(const NodeInstance& inst);

} // namespace chi


#endif // CHI_NODE_COMPILER_HPP
