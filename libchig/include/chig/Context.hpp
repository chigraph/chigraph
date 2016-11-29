#ifndef CHIG_CONTEXT_HPP
#define CHIG_CONTEXT_HPP

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "chig/Fwd.hpp"
#include "chig/ToString.hpp"
#include "chig/json.hpp"

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/Support/raw_os_ostream.h>

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

	/// Gets the module by the name
	/// \param moduleName The name of the module to find
	/// \return ret_module The module that has the name \c moduleName, nullptr if none were found
	ChigModule* getModuleByName(const char* moduleName) noexcept;

	/// Adds a custom module to the Context
	/// \param modToAdd The module to add. The context will take excluseive ownership of it.
	Result addModule(std::unique_ptr<ChigModule> modToAdd) noexcept;

	/// Gets a llvm::Type from a module
	/// \param module The name of the module, "lang" if nullptr
	/// \param name The name of the type, required
	/// \param ret_type The return type
	/// \return The result
	Result getType(const char* module, const char* name, llvm::Type** ret_type) noexcept;

	/// Gets a NodeType from the JSON and name
	/// \param module The module name.
	/// \param name The name of the node type
	/// \param data The JSON data that is used to construct the NodeType.
	Result getNodeType(const char* module, const char* name, const nlohmann::json& data,
		std::unique_ptr<NodeType>* ret_nodetype) noexcept;

	/// Turns a type into a string
	/// \param ty The type to stringify
	/// \return The return string
	std::string stringifyType(llvm::Type* ty);

	llvm::LLVMContext llcontext;  /// The LLVM context to use with everything under the context

	std::vector<std::unique_ptr<ChigModule>> modules;  /// The modules that have been loaded.
	std::vector<std::string> searchPaths;			   /// The places to search for modules

private:
	std::string resolveModulePath(const char* path);
};
}

#endif  // CHIG_CONTEXT_HPP
