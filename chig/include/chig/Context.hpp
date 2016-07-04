#ifndef CHIG_CONTEXT_HPP
#define CHIG_CONTEXT_HPP

#pragma once

#include "chig/ImportedModule.hpp"

#include <string>
#include <vector>
#include <memory>

namespace chig {

struct Context {

	Context() = default;

	// no move or copy, doesn't make sense
	Context(const Context& context) = delete;
	Context(Context&&) = delete;
	
	// there should be a `name`.bc file in one of the search paths
	ImportedModule* loadModule(const char* name);
	void unloadModule(ImportedModule* toUnload);

	ImportedModule* getModuleByName(const char* moduleName);
	
	std::vector<std::unique_ptr<ImportedModule>> modules;
	std::vector<std::string> searchPaths;

	llvm::LLVMContext context;

};

}

#endif // CHIG_CONTEXT_HPP
