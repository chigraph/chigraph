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
struct Graph {
	Graph(){};

	Graph(Context& con, const nlohmann::json& data, Result& res);

	Result toJson(nlohmann::json* toFill) const;

	/// Gets the nodes with a given type
	/// \param module The module the type is in
	/// \param name THe name of the type
	/// \return A vector of NodeInstance
	std::vector<NodeInstance*> getNodesWithType(const char* module, const char* name) const
		noexcept;

	/// Add a node to the graph
	/// \param type The type of the node
	/// \param x The x location of the node
	/// \param y The y location of the node
	/// \param id The node ID
	NodeInstance* insertNode(
		std::unique_ptr<NodeType> type, float x, float y, const std::string& id);

	std::unordered_map<std::string, std::unique_ptr<NodeInstance>> nodes;  /// Storage for the nodes

	Context* context;
};
}

#endif  // CHIG_GRAPH_HPP
