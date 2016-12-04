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

namespace chig
{
/// The class that handles modules
/// It also stores a \c LLVMContext object to be used everywhere.
struct Context {
	/// Creates a context with just the lang module
	/// \param workspacePath Path to the workspace
	Context(const boost::filesystem::path& workPath = {});

	// no move or copy, doesn't make sense
	Context(const Context& context) = delete;
	Context(Context&&) = delete;

	/// Gets the module by the name
	/// \param moduleName The name of the module to find
	/// \return ret_module The module that has the name \c moduleName, nullptr if none were found
	ChigModule* getModuleByName(gsl::cstring_span<> moduleName) noexcept;

	/// Load a module from disk
	/// \param name The name of the moudle
	Result addModule(const gsl::cstring_span<> name);

	/// Load a module from JSON -- avoid this use the string overload
	/// \param json The JSON data
	/// \param name The name of the module, returned. Optional.
	Result addModuleFromJson(const nlohmann::json& json, std::string* name = nullptr);

	/// Adds a custom module to the Context
	/// This usually doesn't get called, use the \c gsl::string_span<> overload instead
	/// \param modToAdd The module to add. The context will take excluseive ownership of it.
	Result addModule(std::unique_ptr<ChigModule> modToAdd) noexcept;

	/// Gets a llvm::Type from a module
	/// \param module The name of the module, "lang" if nullptr
	/// \param name The name of the type, required
	/// \param toFill The \c llvm::Type to fill
	/// \return The result
	Result getType(
		gsl::cstring_span<> module, gsl::cstring_span<> name, llvm::Type** toFill) noexcept;

	/// Gets a NodeType from the JSON and name
	/// \param moduleName The module name.
	/// \param typeName The name of the node type
	/// \param data The JSON data that is used to construct the NodeType.
	/// \param toFill The point to fill
	Result getNodeType(gsl::cstring_span<> moduleName, gsl::cstring_span<> typeName,
		const nlohmann::json& data, std::unique_ptr<NodeType>* toFill) noexcept;

	/// Turns a type into a string
	/// \param ty The type to stringify
	/// \return The return string
	std::string stringifyType(llvm::Type* ty);

	llvm::LLVMContext llcontext;  /// The LLVM context to use with everything under the context

	boost::filesystem::path getWorkspacePath() { return workspacePath; }
	Result compileModule(gsl::cstring_span<> name, std::unique_ptr<llvm::Module>* toFill);

	size_t getNumModules() const { return modules.size(); }
private:
	boost::filesystem::path workspacePath;

	std::vector<std::unique_ptr<ChigModule>> modules;  /// The modules that have been loaded.
};
}

#endif  // CHIG_CONTEXT_HPP
