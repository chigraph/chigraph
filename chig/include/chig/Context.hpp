#ifndef CHIG_CONTEXT_HPP
#define CHIG_CONTEXT_HPP

#pragma once

#include "chig/ImportedModule.hpp"
#include "chig/NodeType.hpp"

#include <string>
#include <vector>
#include <memory>

namespace chig {

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
	
	llvm::LLVMContext context;
	
	std::vector<std::unique_ptr<ImportedModule>> modules;
	std::vector<std::string> searchPaths;

	std::vector<std::unique_ptr<NodeType>> langNodeTypes;
	
private:
	
	std::string resolveModulePath(const char* path);

};

}

#endif // CHIG_CONTEXT_HPP
