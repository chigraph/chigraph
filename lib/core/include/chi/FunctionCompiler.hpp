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

/// Class for compiling `GraphFunctions` into `llvm::Function`s
struct FunctionCompiler {
	/// Constructor
	/// \param func The function to compile
	/// \param moduleToGenInto The module to create the function in
	/// \param debugCU The compile unit we're in, this is a DICompileUnit
	/// \param debugBuilder The Debug information builder for the module
	FunctionCompiler(const GraphFunction& func, LLVMModuleRef moduleToGenInto,
	                 LLVMMetadataRef debugCU, LLVMDIBuilderRef debugBuilder);

	/// Creates the function, but don't actually generate into it
	/// \pre `initialized() == false`
	/// \post `initialized() == true`
	/// \param validate Should the module be validated (using functions in FunctionValidator.hpp)?
	/// \return The Result
	Result initialize(bool validate = true);

	/// Generates the contents of the function
	/// \pre `initialized() == true`
	/// \pre `compiled() == false`
	/// \post `compiled() == true`
	/// \return The Result
	Result compile();

	/// Create the subroutine type for the function
	/// \return The subroutine type
	LLVMMetadataRef createSubroutineType();

	/// Get the debug function.
	/// \pre `initialized() == true`
	/// \return The debug function
	LLVMMetadataRef diFunction() {
		assert(initialized() &&
		       "Please initialize the function compiler before getting the debug function");
		return mDebugFunc;
	}

	/// Get the module being generated
	/// \return The Module
	LLVMModuleRef llvmModule() const { return mModule; }

	/// The debug builder we're using for the module
	/// \return The DIBuilder
	LLVMDIBuilderRef diBuilder() const { return mDIBuilder; }

	/// The compile unit for the module
	/// \return The DICompileUnit
	LLVMMetadataRef debugCompileUnit() const { return mDebugCU; }

	/// The debug file that this function is in
	/// \return The debug file
	LLVMMetadataRef debugFile() const { return mDIFile; }

	/// The block for allocating variables at the beginning of the function
	/// \pre `initialized() == true`
	/// \return The alloc block
	LLVMBasicBlockRef allocBlock() const {
		assert(initialized() &&
		       "Please initialize the function compiler before getting the alloc block");
		return mAllocBlock;
	}

	/// Get the value for a local variable
	/// \pre `initialized() == true`
	/// \return nullptr if it doesn't exist. A value if it's valid.
	LLVMValueRef localVariable(std::string_view name);

	/// Get the llvm function this compiler is generating
	/// \pre `initialized() == true`
	/// \return The function.
	LLVMValueRef llFunction() const {
		assert(initialized() &&
		       "Please initialize the function compiler before trying to get the LLVM function");
		return mLLFunction;
	}

	/// Get the value for the address to jump back to
	/// \pre `initialized() == true`
	/// \return The value
	LLVMValueRef postPureBreak() const {
		assert(initialized() &&
		       "Please initialize the function compiler before trying to get the post-pure break");
		return mPostPureBreak;
	}

	/// Get the graph function
	/// \retrun The GraphFunction
	const GraphFunction& function() const { return *mFunction; }

	/// Get `function().module()`
	/// \return `function().module()`
	GraphModule& module() const;

	/// Get `function().context()`
	/// \return `function().context()`
	Context& context() const;

	/// Get the debug line number for the node instance
	/// Unique for each node.
	/// \pre `&node.function() == &function()`
	/// \param node The instance to get the line number for
	/// \return The line number
	int nodeLineNumber(NodeInstance& node);

	/// Get if the function is initialized (`initialize()` has been called)
	/// \return True if it is, false otherwise
	bool initialized() const { return mInitialized; }

	/// Get if the function has been compiled (`compile()` has been called)
	/// \return True if it has, false otherwise
	bool compiled() const { return mCompiled; }

	/// Get a node compile for a certain node
	/// \pre `&node.function() == &function()`
	/// \return The node compiler for the function
	NodeCompiler* nodeCompiler(NodeInstance& node);

	/// Get or create a node compiler for a node
	/// \pre `&node.function() == &function()`
	/// \return The compiler.
	NodeCompiler* getOrCreateNodeCompiler(NodeInstance& node);

private:
	std::unordered_map<std::string, LLVMValueRef> mLocalVariables;

	LLVMModuleRef    mModule    = nullptr;
	LLVMDIBuilderRef mDIBuilder = nullptr;
	LLVMMetadataRef  mDebugCU   = nullptr;
	LLVMMetadataRef  mDIFile{};
	LLVMMetadataRef  mDebugFunc{};

	const GraphFunction* mFunction = nullptr;

	LLVMValueRef      mLLFunction = nullptr;
	LLVMBasicBlockRef mAllocBlock = nullptr;

	std::unordered_map<NodeInstance*, NodeCompiler> mNodeCompilers;

	boost::bimap<unsigned, NodeInstance*> mNodeLocations;

	bool mInitialized = false;
	bool mCompiled    = false;

	LLVMValueRef mPostPureBreak = nullptr;
};

/// Compile the graph to an \c llvm::Function (usually called from JsonModule::generateModule)
/// \param func The function to compile
/// \param mod The module to codgen into, should already be a valid module
/// \param debugCU The compilation unit that the GraphFunction resides in.
/// \param debugBuilder The debug builder to build debug info
/// \return The result
Result compileFunction(const GraphFunction& func, LLVMModuleRef mod, LLVMMetadataRef debugCU,
                       LLVMDIBuilderRef debugBuilder);
}  // namespace chi

#endif  // CHI_FUNCTION_COMPILER_HPP
