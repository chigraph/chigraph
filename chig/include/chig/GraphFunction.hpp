#ifndef CHIG_GRAPH_FUNCTION_HPP
#define CHIG_GRAPH_FUNCTION_HPP

#pragma once

#include "chig/NodeInstance.hpp"
#include "chig/json.hpp"

#include <llvm/IR/Module.h>

#include <string>
#include <vector>

namespace chig {

/// this is an AST-like representation of a function in a graph
/// It is used for IDE-like behavior, codegen, and JSON generation.
struct GraphFunction {
	
	/// Construct the graph
	/// Also constructs a input node
	/// \param name The name of the function
	/// \param inputs The types and descriptions of the inputs
	/// \param outputs The types and descriptions of the outputs
	GraphFunction(std::string name, const std::vector<std::pair<llvm::Type*, std::string>>& inputs, const std::vector<std::pair<llvm::Type*, std::string>>& outputs);
	
	/// Constructs a GraphFunction from a JOSN object
	/// \param j The JSON object to read from
	/// \return The GraphFunction that has been produced
	static GraphFunction fromJSON(const nlohmann::json& j);
	
	/// Serialize the GraphFunction to JSON
	/// \return The JSON object representing the graph
	nlohmann::json toJSON();
	
	/// Compile the graph to an \c llvm::Function
	llvm::Function* compile();
	
	/// Add a node to the graph
	/// \param type The type of the node
	/// \param x The x location of the node
	/// \param y The y location of the node
	NodeInstance* insertNode(std::unique_ptr<NodeType> type, float x, float y);
	
	std::string graphName; /// the name of the function
	
	std::vector<std::unique_ptr<NodeInstance>> nodes; /// Storage for the nodes
	NodeInstance* entry; /// A convenience pointer holding the entry point of the function.
	
	std::vector<std::pair<llvm::Type*, std::string>> outputs; /// The output types + descriptions
	
};

}

#endif // CHIG_GRAPH_FUNCTION_HPP
