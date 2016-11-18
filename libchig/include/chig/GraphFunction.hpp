#ifndef CHIG_GRAPH_FUNCTION_HPP
#define CHIG_GRAPH_FUNCTION_HPP

#pragma once

#include "chig/Fwd.hpp"
#include "chig/ToString.hpp"
#include "chig/json.hpp"

#include <llvm/IR/Module.h>

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <boost/optional.hpp>

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
	
	/// Destructor
	~GraphFunction();

	/// Constructs a GraphFunction from a JOSN object
	/// \param j The JSON object to read from
	/// \context The context to create the GraphFunction with
	/// \param ret_func The GraphFunction that has been produced
	/// \return The result
	static Result fromJSON(
		Context& context, const nlohmann::json& j, std::unique_ptr<GraphFunction>* ret_func);

	/// Serialize the GraphFunction to JSON
	/// \param toFill The JSON object representing the graph
	/// \return The result
	Result toJSON(nlohmann::json* toFill) const;

	/// Compile the graph to an \c llvm::Function
	/// Throws on error
	/// \param module The module to codgen into
	/// \param ret The \c llvm::Function that it was compiled to
	/// \ret The result
	Result compile(llvm::Module* module, llvm::Function** ret) const;

	/// Gets the node with type lang:entry
	/// returns {nullptr, ~0} on failure
	/// Also returns {nullptr, ~0} if there are two entry nodes, which is illegal
	/// \return Entry node
	NodeInstance* getEntryNode() const noexcept;

	/// Gets the nodes with a given type
	/// \param module The module the type is in
	/// \param name THe name of the type
	/// \return A vector of NodeInstance
	std::vector<NodeInstance*> getNodesWithType(
		const char* module, const char* name) const noexcept;

	/// Gets the return type, based on the exit nodes
	/// \return The type of the return, or {} if there are multiple or if there are none
	boost::optional<std::vector<llvm::Type*>> getReturnTypes() const noexcept;

	/// Add a node to the graph
	/// \param type The type of the node
	/// \param x The x location of the node
	/// \param y The y location of the node
	/// \param id The node ID
	NodeInstance* insertNode(std::unique_ptr<NodeType> type, float x, float y, const std::string& id);

	std::string graphName;  /// the name of the function

	std::unordered_map<std::string, std::unique_ptr<NodeInstance>> nodes;  /// Storage for the nodes

	Context* owningContext;
};
}

#endif  // CHIG_GRAPH_FUNCTION_HPP
