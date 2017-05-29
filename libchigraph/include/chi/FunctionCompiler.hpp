/// \file chi/FunctionCompiler.hpp
/// Defines functions for compiling GraphFunction objects

#pragma once

#ifndef CHI_FUNCTION_COMPILER_HPP
#define CHI_FUNCTION_COMPILER_HPP

#include "chi/Fwd.hpp"

#include <memory>

namespace chi {

struct FunctionCompiler {
	
	FunctionCompiler(const GraphFunction& func, llvm::Module& moduleToGenInto, llvm::DICompileUnit& debugCU, llvm::DIBuilder& debugBuilder);
	
	Result compile(bool validate = true);
	
	void createSubroutineType();
	
	llvm::Module& llModule() const { return *mModule; }
	llvm::DIBuilder& diBuilder() const { return *mDIBuilder; }
	llvm::DICompileUnit& debugCompileUnit() const { return *mDebugCU; }
	
	const GraphFunction& function() const { return *mFunction; }
	Context& context() const { return *mContext; }
	
private:
	
	llvm::Module* mModule;
	llvm::DIBuilder* mDIBuilder;
	llvm::DICompileUnit* mDebugCU;
	
	const GraphFunction* mFunction;
	Context* mContext;
	
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
