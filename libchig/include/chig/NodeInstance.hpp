#ifndef CHIG_NODE_INSTANCE_HPP
#define CHIG_NODE_INSTANCE_HPP

#pragma once

#include "chig/NodeType.hpp"

#include <vector>

namespace chig
{
struct NodeInstance {
	NodeInstance(std::unique_ptr<NodeType> nodeType, float x, float y, std::string id);

	NodeInstance(const NodeInstance&) = default;
	NodeInstance(NodeInstance&&) = default;

	// general data
	std::unique_ptr<NodeType> type = nullptr;

	float x = 0.f;
	float y = 0.0;

	// connections
	std::vector<std::vector<std::pair<NodeInstance*, unsigned int>>> inputExecConnections;
	std::vector<std::pair<NodeInstance*, unsigned int>> inputDataConnections;
	std::vector<std::pair<NodeInstance*, unsigned int>> outputExecConnections;
	std::vector<std::vector<std::pair<NodeInstance*, unsigned int>>> outputDataConnections;

	std::string id;
};

/// Connects two nodes' data connections
/// \param lhs The node to the left, the node outputting the data
/// \param connectionInputID The ID of data connection in \c lhs to be connected
/// \param rhs The node to the right, that takes in the data as a parameter
/// \param connectionOutputID The ID of data input in \c rhs
Result connectData(
	NodeInstance& lhs, size_t connectionInputID, NodeInstance& rhs, size_t connectionOutputID);

/// Connects two nodes' exec connections
/// \param lhs The node to the left, the node outputting the connections
/// \param connectionInputID The ID of exec connection in \c lhs to be connected
/// \param rhs The node to the right, that takes in the exec as a parameter
/// \param connectionOutputID The ID of exec input in \c rhs
Result connectExec(
	NodeInstance& lhs, size_t connectionInputID, NodeInstance& rhs, size_t connectionOutputID);
}

#endif  // CHIG_NODE_INSTANCE_HPP
