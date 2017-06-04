/// \file chi/FunctionCompiler.hpp
/// Defines functions for compiling GraphFunction objects

#pragma once

#ifndef CHI_FUNCTION_COMPILER_HPP
#define CHI_FUNCTION_COMPILER_HPP

#include "chi/Fwd.hpp"
#include "chi/NodeCompiler.hpp"

#include <memory>
#include <unordered_map>

#include <boost/bimap.hpp>

namespace chi {

struct FunctionCompiler {
	
	FunctionCompiler(const GraphFunction& func, llvm::Module& moduleToGenInto, llvm::DICompileUnit& debugCU, llvm::DIBuilder& debugBuilder);
	
	/// Creates the function, but don't actually generate into it
	/// \pre `initialized() == false`
	/// \post `initialized() == true`
	Result initialize(bool validate = true);
	
	/// Generates the contents of the function
	/// \pre `initialized() == true`
	/// \pre `compiled() == false`
	/// \post `compiled() == true`
	Result compile();
	
	llvm::DISubroutineType* createSubroutineType();
	
	llvm::DISubprogram& diFunction() const { return *mDebugFunc; }
	llvm::Module& llModule() const { return *mModule; }
	llvm::DIBuilder& diBuilder() const { return *mDIBuilder; }
	llvm::DICompileUnit& debugCompileUnit() const { return *mDebugCU; }
	llvm::BasicBlock& allocBlock() const { return *mAllocBlock; }
	
	
	/// \pre `initialized() == true`
	llvm::Function& llFunction() const { assert(initialized() && "Please initialize the function compiler before trying to get the LLVM function") ;return *mLLFunction; }
	
	/// \pre `compiled() == true` 
	llvm::Value& postPureBreak() const { assert(initialized() && "Please initialize the function compiler before trying to get the post-pure break"); return *mPostPureBreak; }
	
	const GraphFunction& function() const { return *mFunction; }
	GraphModule& module() const;
	Context& context() const;
	
	int nodeLineNumber(NodeInstance& node);
	
	bool initialized() const { return mInitialized; }
	bool compiled() const { return mCompiled; }
	
	/// Get a node compile for a certain (nonpure) node
	/// \pre `compiled() == true` 
	/// \pre `&inst.function() == &function()`
	NodeCompiler& nodeCompilerForNode(NodeInstance& inst) const;
	
	NodeCompiler& getOrCompileNode(NodeInstance& inst);
	
private:
	
	llvm::Module* mModule;
	llvm::DIBuilder* mDIBuilder;
	llvm::DICompileUnit* mDebugCU;
	llvm::DISubprogram* mDebugFunc;
	
	const GraphFunction* mFunction;
	
	llvm::Function* mLLFunction;
	llvm::BasicBlock*   mAllocBlock;
	llvm::DISubprogram* mDISubprogram;
	
	std::unordered_map<NodeInstance*, NodeCompiler> mNodeCompilers;
	
	boost::bimap<unsigned, NodeInstance*>                  mNodeLocations;
	std::unordered_map<std::string, std::shared_ptr<void>> mCompileCache;
	
	bool mInitialized = false;
	bool mCompiled = false;
	
	llvm::Value* mPostPureBreak = nullptr;
	
};

/// Compile the graph to an \c llvm::Function (usually called from JsonModule::generateModule)
/// \param func The function to compile
/// \param mod The module to codgen into, should already be a valid module
/// \param debugCU The compilation unit that the GraphFunction resides in.
/// \param debugBuilder The debug builder to build debug info
/// \return The result
Result compileFunction(const GraphFunction& func, llvm::Module* mod, llvm::DICompileUnit* debugCU,
                       llvm::DIBuilder& debugBuilder);
}  // namespace chi

#endif  // CHI_FUNCTION_COMPILER_HPP
