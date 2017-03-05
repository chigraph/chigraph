/// \file chig/Context.hpp
/// Defines the Context class and related functions

#ifndef CHIG_CONTEXT_HPP
#define CHIG_CONTEXT_HPP

#pragma once

#include <memory>
#include <unordered_map>
#include <unordered_set>

#include "chig/Fwd.hpp"
#include "chig/ToString.hpp"
#include "chig/json.hpp"

#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>

#include <gsl/gsl>

#include <boost/filesystem.hpp>
#include <boost/optional.hpp>

namespace chig {
/// The class that handles the loading, creation, storing, and compilation of modules
/// It also stores a \c LLVMContext object to be used everywhere.
struct Context {
	/// Creates a context with just the lang module
	/// \param workPath Path to the workspace, or a subdirectory of the workspace
	Context(const boost::filesystem::path& workPath = {});

	// no move or copy, doesn't make sense
	Context(const Context& context) = delete;
	Context(Context&&)              = delete;

	/// Gets the module by the full name
	/// \param fullModuleName The name of the module to find
	/// \return ret_module The module that has the full name \c fullModuleName, nullptr if none were
	/// found
	ChigModule* moduleByFullName(const boost::filesystem::path& fullModuleName) const noexcept;

	/// Create a new GraphModule with the given full name
	/// \param fullName The new name
	/// \return The created GraphModule
	GraphModule* newGraphModule(const boost::filesystem::path& fullName);

	/// Get the list of modules in the workspace
	/// \return The module list
	std::vector<std::string> listModulesInWorkspace() const noexcept;

	/// Load a module from disk
	/// \param name The name of the moudle
	/// \retval toFill The module that was loaded, optional
	/// \return The result
	Result loadModule(const boost::filesystem::path& name, ChigModule** toFill = nullptr);

	/// Load a module from JSON -- avoid this use the string overload
	/// \param fullName The full path of the module, including URL
	/// \param json The JSON data
	/// \retval toFill The GraphModule* to fill into, optional
	/// \return The Result
	Result addModuleFromJson(const boost::filesystem::path& fullName, const nlohmann::json& json,
	                         GraphModule** toFill = nullptr);

	/// Adds a custom module to the Context
	/// This usually doesn't get called, use the \c gsl::string_span<> overload instead
	/// \param modToAdd The module to add. The context will take excluseive ownership of it.
	/// \return True if the module was added (it didn't exist before)
	bool addModule(std::unique_ptr<ChigModule> modToAdd) noexcept;

	/// Unloads a module
	/// \param fullName The full name of the module to unload
	/// \return True if a module was unloaded
	bool unloadModule(const boost::filesystem::path& fullName);

	/// Gets a DataType from a module
	/// \param module The full name of the module
	/// \param name The name of the type, required
	/// \retval toFill The type to fill
	/// \return The result
	Result typeFromModule(const boost::filesystem::path& module, gsl::cstring_span<> name,
	                      DataType* toFill) noexcept;

	/// Gets a NodeType from the JSON and name
	/// \param moduleName The full module name.
	/// \param typeName The name of the node type
	/// \param data The JSON data that is used to construct the NodeType.
	/// \retval toFill The point to fill
	/// \pre toFill isn't null (the value the unique_ptr points to be can be null, but not the
	/// pointer to the unique_ptr)
	/// \return The Result
	Result nodeTypeFromModule(const boost::filesystem::path& moduleName,
	                          gsl::cstring_span<> typeName, const nlohmann::json& data,
	                          std::unique_ptr<NodeType>* toFill) noexcept;

	/// Get the workspace path of the Context
	/// \return The workspace path
	boost::filesystem::path workspacePath() const { return mWorkspacePath; }
	/// Check if this context has a workspace bound to it -- same as !workspacePath().empty()
	/// \return If it has a workspace
	bool hasWorkspace() const noexcept { return !workspacePath().empty(); }
	
	/// Compile a module to a \c llvm::Module
	/// \param fullName The full name of the module to compile
	/// \retval toFill The \c llvm::Module to fill -- this can be nullptr it will be replaced
	/// \pre toFill isn't null (the value the unique_ptr points to be can be null, but not the
	/// pointer to the unique_ptr)
	/// \pre moduleByFullName(fullName) exists
	/// \return The result
	Result compileModule(const boost::filesystem::path& fullName,
	                     std::unique_ptr<llvm::Module>* toFill);
	
	/// Compile a module to a \c llvm::Module
	/// \param module The module to compile
	/// \retval toFill The \c llvm::Module to fill -- this can be nullptr it will be replaced
	/// \pre toFill isn't null (the value the unique_ptr points to be can be null, but not the
	/// pointer to the unique_ptr)
	/// \return The result
	Result compileModule(ChigModule& mod,
	                     std::unique_ptr<llvm::Module>* toFill);

	/// Get the number of modules this Context has
	/// \return The module count
	size_t numModules() const { return mModules.size(); }
	/// Get the associated LLVMContext
	/// \return The LLVMContext
	llvm::LLVMContext& llvmContext() const { return *mLLVMContext; }

	/// Get the LangModule, if it has been loaded
	/// \return The LangModule
	LangModule* langModule() const { return mLangModule; }

	/// Get the CModule, if it has been loaded
	/// \return The CModule
	CModule* cModule() const { return mCModule; }

private:
	/// The workspace path for the module
	boost::filesystem::path mWorkspacePath;

	/// The LLVM context to use with everything under the context
	std::unique_ptr<llvm::LLVMContext> mLLVMContext;

	/// The modules that have been loaded.
	std::vector<std::unique_ptr<ChigModule>> mModules;

	// This cache is only for use during compilation to not duplicate modules
	std::unordered_map<std::string /*full name*/, llvm::Module* /*the compiled module*/>
	    mCompileCache;

	CModule*    mCModule    = nullptr;
	LangModule* mLangModule = nullptr;
};

/// Get the workspace directory from a child of the workspace directory
/// Example: say you have a workspace at ~/chig/
/// If you used this with ~/chig/src/ it would return ~/chig/
/// \param path The child path
/// \return The workspace path, or an empty path if it wasn't found
boost::filesystem::path workspaceFromChildPath(const boost::filesystem::path& path);

/// Turns a type into a string
/// \param ty The type to stringify
/// \return The return string
std::string stringifyLLVMType(llvm::Type* ty);

/// Interpret LLVM IR, just a convenience function
/// \param mod The LLVM Module to interpret
/// \parmam funcToRun The function to run, or leave a nullptr for main
Result interpretLLVMIR(std::unique_ptr<llvm::Module>   mod,
                       llvm::CodeGenOpt::Level         optLevel = llvm::CodeGenOpt::Default,
                       std::vector<llvm::GenericValue> args = {}, llvm::GenericValue* ret = nullptr,
                       llvm::Function* funcToRun = nullptr);
}

#endif  // CHIG_CONTEXT_HPP
