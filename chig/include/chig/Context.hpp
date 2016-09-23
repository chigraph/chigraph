#ifndef CHIG_CONTEXT_HPP
#define CHIG_CONTEXT_HPP

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "chig/ChigModule.hpp"

#include <llvm/IR/LLVMContext.h>

namespace chig
{
/// The class that handles modules
/// It also stores a \c LLVMContext object to be used everywhere.
struct Context {
	/// Creates a context with just the lang module
	///
	Context();

	// no move or copy, doesn't make sense
	Context(const Context& context) = delete;
	Context(Context&&) = delete;

	/// Unloads a module
	/// \param toUnload The module to unload; must be in \c modules
	/// \return true if the module was found and unloaded, false if it was not found and nothing
	/// happened
	bool unloadModule(ChigModule* toUnload);

	/// Gets the module by the name
	/// \param moduleName The name of the module to find
	/// \return The module that has the name \c moduleName, nullptr if none were found
	ChigModule* getModuleByName(const char* moduleName);

	/// Adds a custom module to the Context
	/// \param modToAdd The module to add. The context will take excluseive ownership of it.
	void addModule(std::unique_ptr<ChigModule> modToAdd);

	llvm::LLVMContext context;  /// The LLVM context to use with everything under the context

	std::vector<std::unique_ptr<ChigModule>> modules;  /// The modules that have been loaded.
	std::vector<std::string> searchPaths;			   /// The places to search for modules

	/// Gets a llvm::Type from a module
	/// \param module The name of the module, "lang" if nullptr
	/// \param name The name of the type, required
	llvm::Type* getType(const char* module, const char* name);

	/// Gets a NodeType from the JSON and name
	/// \param module The module name.
	/// \param name The name of the node type
	/// \param data The JSON data that is used to construct the NodeType.
	std::unique_ptr<NodeType> getNodeType(
		const char* module, const char* name, const nlohmann::json& data = {});

	/// Turns a type into a string
	/// \ty The type to stringify
	std::string stringifyType(llvm::Type* ty);

private:
	std::string resolveModulePath(const char* path);
};
}

#endif  // CHIG_CONTEXT_HPP
