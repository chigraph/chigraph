#ifndef CHIG_MODULE_HPP
#define CHIG_MODULE_HPP

#pragma once

#include <llvm/IR/Module.h>

namespace chig{

struct NodeType;

struct ImportedModule {

	ImportedModule(std::unique_ptr<llvm::Module> module);
	~ImportedModule();

	std::unique_ptr<llvm::Module> module;

	std::vector<std::unique_ptr<NodeType>> nodes;

};

}

#endif // CHIG_MODULE_HPP
