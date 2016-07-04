#ifndef CHIG_GRAPH_FUNCTION_HPP
#define CHIG_GRAPH_FUNCTION_HPP

#pragma once

#include "chig/NodeInstance.hpp"
#include "chig/json.hpp"

#include <llvm/IR/Module.h>

#include <string>

namespace chig {

// this is an AST-like representation of a function in a graph
struct GraphFunction {
	
	GraphFunction(const nlohmann::json& data);
	GraphFunction(){};
	
	nlohmann::json toJSON();
	
	llvm::Module* compile();
	
	NodeInstance* insertNode(NodeType* type, float x, float y);
	
	std::string graphName;
	
	std::vector<std::unique_ptr<NodeInstance>> nodes;
	NodeInstance* entry;
	
	std::vector<llvm::Type*> inputs;
	std::vector<llvm::Type*> outputs;
	
};

}

#endif // CHIG_GRAPH_FUNCTION_HPP
