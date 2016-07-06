#ifndef CHIG_CONTEXT_HPP
#define CHIG_CONTEXT_HPP

#pragma once

#include "chig/ImportedModule.hpp"
#include "chig/NodeType.hpp"

#include <string>
#include <vector>
#include <memory>

namespace chig {

/// The class that handles modules
/// It also stores a \c LLVMContext object to be used everywhere. 
struct Context {

	Context();

	// no move or copy, doesn't make sense
	Context(const Context& context) = delete;
	Context(Context&&) = delete;
	
	/// Loads a module from disk. The module on disk must be a \c .bc file 
	/// in either the working Direcotry or in on of the search paths in \c searchPaths. 
	/// \param name The name of the module to load
	/// \return The module that was loaded. This is already stored in the \c modules vector.
	ImportedModule* loadModule(const char* name);
	
	/// Unloads a module
	/// \param toUnload The module to unload; must be in \c modules
	void unloadModule(ImportedModule* toUnload);

	/// Gets the module by the name
	/// \param 
	ImportedModule* getModuleByName(const char* moduleName);
	
	llvm::LLVMContext context; /// The LLVM context to use with everything under the context
	
	std::vector<std::unique_ptr<ImportedModule>> modules; /// The modules that have been loaded. 
	std::vector<std::string> searchPaths; /// The places to search for modules
	
private:
	
	std::string resolveModulePath(const char* path);

};

}

#endif // CHIG_CONTEXT_HPP
