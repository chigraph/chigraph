#ifndef CHIG_MODULE_HPP
#define CHIG_MODULE_HPP

#pragma once

#include <llvm/IR/Module.h>

namespace chig{

struct NodeType;

/// A module imported from a \c llvm::Module*
/// Finds the nodes in an \c llvm::Module* and exposes them
struct ImportedModule {

	/// Create a \c ImportedModule from a llvm::Module*. 
	/// This object will take exclusive ownership over the object
	ImportedModule(std::unique_ptr<llvm::Module> module);
	~ImportedModule();

	std::unique_ptr<llvm::Module> module; /// The \c llvm::Module that it represents

	std::vector<std::unique_ptr<NodeType>> nodes; /// The nodes that the module contains

};

}

#endif // CHIG_MODULE_HPP
