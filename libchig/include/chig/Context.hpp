#ifndef CHIG_CONTEXT_HPP
#define CHIG_CONTEXT_HPP

#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "chig/Fwd.hpp"
#include "chig/ToString.hpp"
#include "chig/json.hpp"

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/Support/raw_os_ostream.h>

#include <gsl/gsl>

#include <boost/filesystem.hpp>
#include <boost/optional.hpp>

namespace chig
{
/// The class that handles the loading, creation, storing, and compilation of modules
/// It also stores a \c LLVMContext object to be used everywhere.
struct Context {
	/// Creates a context with just the lang module
	/// \param workPath Path to the workspace, or a subdirectory of the workspace
	Context(const boost::filesystem::path& workPath = {});

	// no move or copy, doesn't make sense
	Context(const Context& context) = delete;
	Context(Context&&) = delete;

	/// Gets the module by the name
	/// \param moduleName The name of the module to find
	/// \return ret_module The module that has the name \c moduleName, nullptr if none were found
	ChigModule* moduleByName(gsl::cstring_span<> moduleName) const noexcept;

	/// Gets the module by the full name
	/// \param fullModuleName The name of the module to find
	/// \return ret_module The module that has the full name \c fullModuleName, nullptr if none were
	/// found
	ChigModule* moduleByFullName(gsl::cstring_span<> fullModuleName) const noexcept;

	/// Load a module from disk
	/// \param name The name of the moudle
	Result addModule(const gsl::cstring_span<> name);

	/// Load a module from JSON -- avoid this use the string overload
	/// \param fullName The full path of the module, including URL
	/// \param json The JSON data
	/// \param name The name of the module, returned. Optional.
	Result addModuleFromJson(
		gsl::cstring_span<> fullName, const nlohmann::json& json, JsonModule** toFill = nullptr);

	/// Adds a custom module to the Context
	/// This usually doesn't get called, use the \c gsl::string_span<> overload instead
	/// \param modToAdd The module to add. The context will take excluseive ownership of it.
	Result addModule(std::unique_ptr<ChigModule> modToAdd) noexcept;

	/// Gets a llvm::Type from a module
	/// \param module The name of the module, "lang" if nullptr
	/// \param name The name of the type, required
	/// \param toFill The type to fill
	/// \return The result
	Result typeFromModule(
		gsl::cstring_span<> module, gsl::cstring_span<> name, DataType* toFill) noexcept;

	/// Gets a NodeType from the JSON and name
	/// \param moduleName The module name.
	/// \param typeName The name of the node type
	/// \param data The JSON data that is used to construct the NodeType.
	/// \param toFill The point to fill
	Result nodeTypeFromModule(gsl::cstring_span<> moduleName, gsl::cstring_span<> typeName,
		const nlohmann::json& data, std::unique_ptr<NodeType>* toFill) noexcept;

	/// Turns a type into a string
	/// \param ty The type to stringify
	/// \return The return string
	std::string stringifyType(llvm::Type* ty);

	/// Get the workspace path of the Context
	/// \return The workspace path
	boost::filesystem::path workspacePath() const { return mWorkspacePath; }
	/// Compile a module to a \c llvm::Module
	/// \param name The full name of the moudle to compile
	/// \param toFill The \c llvm::Module to fill -- this can be nullptr it will be replaced
	/// \return The result
	Result compileModule(gsl::cstring_span<> fullName, std::unique_ptr<llvm::Module>* toFill);

	/// Get the full module name from a short one
	/// \param shortName The short name of the module
	/// \return The long name. If there was an error getting the module then {}
	boost::optional<std::string> fullModuleName(gsl::cstring_span<> shortName) const;

	/// Get the number of modules this Context has
	/// \return The module count
	size_t numModules() const { return mModules.size(); }
	/// Get the associated LLVMContext
	/// \return The LLVMContext
	llvm::LLVMContext& llvmContext() const { return *mLLVMContext; }
private:
	boost::filesystem::path mWorkspacePath;

	std::unique_ptr<llvm::LLVMContext>
		mLLVMContext;  /// The LLVM context to use with everything under the context
	std::vector<std::unique_ptr<ChigModule>> mModules;  /// The modules that have been loaded.

	// This cache is only for use during compilation to not duplicate modules
	std::unordered_map<std::string /*full name*/, llvm::Module* /*the compiled module*/>
		mCompileCache;
};
}

#endif  // CHIG_CONTEXT_HPP
