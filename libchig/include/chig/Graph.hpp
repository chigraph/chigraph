#pragma once

#ifndef CHIG_GRAPH_HPP
#define CHIG_GRAPH_HPP

#include <unordered_map>

#include "chig/Context.hpp"
#include "chig/NodeInstance.hpp"
#include "chig/Result.hpp"
#include "chig/ToString.hpp"
#include "chig/json.hpp"

namespace chig
{
/// A graph holding nodes
/// Normally not used by the user, used through GraphFunction
struct Graph {
	/// Default constructor, creates an empty graph
	Graph(){};

	/// Normal constructor (don't call this, let GraphFunction call it)
	/// \param con The context object
	/// \param data The JSON data
	/// \param res The result to fill in the case of errors
	Graph(Context& con, const nlohmann::json& data, Result& res);

	/// Convert to JSON. Don't actually call this, let GraphFunction
	/// \param toFill The JSON object to fill.
	/// \return The result
	Result toJson(nlohmann::json* toFill) const;

	/// Gets the nodes with a given type
	/// \param module The module the type is in
	/// \param name THe name of the type
	/// \return A vector of NodeInstance
	std::vector<NodeInstance*> nodesWithType(
		gsl::cstring_span<> module, gsl::cstring_span<> name) const noexcept;

	/// Add a node to the graph
	/// \param type The type of the node
	/// \param x The x location of the node
	/// \param y The y location of the node
	/// \param id The node ID
	/// \param toFill The NodeInstance object to fill into, optional
	/// \return The Result.
	Result insertNode(std::unique_ptr<NodeType> type, float x, float y, gsl::cstring_span<> id,
		NodeInstance** toFill = nullptr);

	/// Get the context
	/// \return The context
	Context& context() const { return *mContext; }

	/// Get the nodes in the function
	/// Usually called by connectData or connectExec or GraphFunction
	/// \return The nodes, mapped by id, value
	std::unordered_map<std::string, std::unique_ptr<NodeInstance>>& nodes() { return mNodes; }
private:
	std::unordered_map<std::string, std::unique_ptr<NodeInstance>>
		mNodes;  /// Storage for the nodes

	Context* mContext;
};
}

#endif  // CHIG_GRAPH_HPP
