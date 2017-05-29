/// \file PureCompiler.hpp

#pragma once

#ifndef CHI_PURE_COMPILER_HPP
#define CHI_PURE_COMPILER_HPP

#include <chi/Fwd.hpp>

#include <unordered_map>

namespace chi {

/// Class to help out with codegenning pure nodes
struct PureCompiler {
	
	/// Construct a pure compiler
	/// \param func The function being generated
	/// \param codegenInto The Module being generated
	PureCompiler(const GraphFunction& func, llvm::Module& codegenInto);
	
	// move only
	PureCompiler(const PureCompiler&) = delete;
	PureCompiler(PureCompiler&&) = default;
	PureCompiler& operator=(const PureCompiler&) = delete;
	PureCompiler& operator=(PureCompiler&&) = default;
	
	
	void updatePure(NodeInstance& dependentNode, NodeInstance& pureNode, llvm::BasicBlock& codegenInto);
	
	const GraphFunction& function() const { return *mFunction; }
	Context& context() const { return *mContext; }
	
private:
	
	llvm::Module* mLLModule;
	const GraphFunction* mFunction;
	Context* mContext;
	
	std::unordered_map<NodeInstance*, llvm::BasicBlock*> mCodegennedNodes;
};

} // namespace chi

#endif // CHI_PURE_COMPILER_HPP
