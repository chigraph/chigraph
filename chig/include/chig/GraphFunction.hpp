#ifndef CHIG_GRAPH_FUNCTION_HPP
#define CHIG_GRAPH_FUNCTION_HPP

#pragma once

#include "chig/NodeInstance.hpp"
#include "chig/json.hpp"

#include <llvm/IR/Module.h>

#include <string>
#include <vector>

namespace chig {

// this is an AST-like representation of a function in a graph
struct GraphFunction {
	
	GraphFunction(std::string name, const std::vector<std::pair<llvm::Type*, std::string>>& inputs, const std::vector<std::pair<llvm::Type*, std::string>>& outputs);
	
	static GraphFunction fromJSON(const nlohmann::json& j);
	
	nlohmann::json toJSON();
	
	llvm::Module* compile();
	
	NodeInstance* insertNode(NodeType* type, float x, float y);
	
	std::string graphName;
	
	std::vector<std::unique_ptr<NodeInstance>> nodes;
	NodeInstance* entry;
	
	std::vector<std::pair<llvm::Type*, std::string>> outputs;
	
};

}

#endif // CHIG_GRAPH_FUNCTION_HPP
