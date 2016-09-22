#ifndef CHIG_GRAPH_FUNCTION_HPP
#define CHIG_GRAPH_FUNCTION_HPP

#pragma once

#include "chig/Context.hpp"
#include "chig/NodeInstance.hpp"
#include "chig/json.hpp"

#include <llvm/IR/Module.h>

#include <string>
#include <vector>
#include <unordered_map>

namespace chig
{
/// this is an AST-like representation of a function in a graph
/// It is used for IDE-like behavior, codegen, and JSON generation.
struct GraphFunction {
	/// Construct the graph
	/// Also constructs a input node
	/// \param context The context
	/// \param name The name of the function
	GraphFunction(Context& context, std::string name);

	/// Constructs a GraphFunction from a JOSN object
	/// \param j The JSON object to read from
	/// \context The context to create the GraphFunction with
	/// \return The GraphFunction that has been produced
	static GraphFunction fromJSON(Context& context, const nlohmann::json& j);

	/// Serialize the GraphFunction to JSON
	/// \return The JSON object representing the graph
	nlohmann::json toJSON();

	/// Compile the graph to an \c llvm::Function
	/// Throws on error
	/// \param module The module to codgen into
	/// \return The \c llvm::Function that it was compiled to
	llvm::Function* compile(llvm::Module* module);
	
	/// Gets the node with type lang:entry
	/// returns {nullptr, ~0} on failure
	/// Also returns {nullptr, ~0} if there are two entry nodes, which is illegal
	/// \return {Entry node, ID in node array}
	std::pair<NodeInstance*, size_t> getEntryNode() noexcept;

	/// Add a node to the graph
	/// \param type The type of the node
	/// \param x The x location of the node
	/// \param y The y location of the node
	NodeInstance* insertNode(std::unique_ptr<NodeType> type, float x, float y);

	std::string graphName;  /// the name of the function

	std::vector<std::unique_ptr<NodeInstance>> nodes;  /// Storage for the nodes

	Context* owningContext;
private:
	
};
}

#endif  // CHIG_GRAPH_FUNCTION_HPP
