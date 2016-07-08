#ifndef CHIG_IMPORTED_MODULE_HPP
#define CHIG_IMPORTED_MODULE_HPP

#pragma once

#include "chig/ChigModule.hpp"

#include <llvm/IR/Module.h>


namespace chig{

struct NodeType;

/// A module imported from a \c llvm::Module*
/// Finds the nodes in an \c llvm::Module* and exposes them
struct ImportedModule : ChigModule {

	/// Create a \c ImportedModule from a llvm::Module*. 
	/// This object will take exclusive ownership over the object
	ImportedModule(std::unique_ptr<llvm::Module> module);
	
	virtual ~ImportedModule();
	
	std::unique_ptr<NodeType> createNodeType(const char * name, const nlohmann::json & json_data) override;
	std::vector<std::function<std::unique_ptr<NodeType>(const nlohmann::json &)> > getNodeTypes() override;
	
	std::unique_ptr<llvm::Module> module; /// The \c llvm::Module that it represents

	std::vector<std::unique_ptr<FunctionCallNodeType>> nodes; /// The nodes that the module contains

};

}

#endif // CHIG_IMPORTED_MODULE_HPP
