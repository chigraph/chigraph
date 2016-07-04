#ifndef CHIG_MODULE_HPP
#define CHIG_MODULE_HPP

#pragma once

#include "chig/NodeType.hpp"

#include <llvm/IR/Module.h>

namespace chig{

struct ImportedModule {

	ImportedModule(std::unique_ptr<llvm::Module> module);

	std::unique_ptr<llvm::Module> module;

	std::vector<NodeType> nodes;

};

}

#endif // CHIG_MODULE_HPP
