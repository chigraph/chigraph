#ifndef CHIG_NODE_INSTANCE_HPP
#define CHIG_NODE_INSTANCE_HPP

#pragma once

#include "chig/NodeType.hpp"

#include <vector>

namespace chig
{
struct NodeInstance {
	NodeInstance(std::unique_ptr<NodeType> nodeType, float x, float y);

	NodeInstance(const NodeInstance&) = default;
	NodeInstance(NodeInstance&&) = default;

	// general data
	std::unique_ptr<NodeType> type = nullptr;

	float x = 0.f;
	float y = 0.0;

	// connections
	std::vector<std::pair<NodeInstance*, unsigned int>> inputExecConnections;
	std::vector<std::pair<NodeInstance*, unsigned int>> inputDataConnections;
	std::vector<std::pair<NodeInstance*, unsigned int>> outputExecConnections;
	std::vector<std::pair<NodeInstance*, unsigned int>> outputDataConnections;
};

/// Connects two nodes' data connections
/// \param lhs The node to the left, the node outputting the data
/// \param connectionInputID The ID of data connection in \c lhs to be connected
/// \param rhs The node to the right, that takes in the data as a parameter
/// \param connectionOutputID The ID of data input in \c rhs
inline void connectData(
	NodeInstance& lhs, size_t connectionInputID, NodeInstance& rhs, size_t connectionOutputID)
{
	// make sure the connection exists
	// the input to the connection is the output to the node
	if (connectionInputID >= lhs.outputDataConnections.size()) {
		throw std::runtime_error(
			"Out of bounds in data connection: there is no output data dock with id " +
			std::to_string(connectionInputID) + " in node with type " + lhs.type->module + ':' +
			lhs.type->name);
	}
	if (connectionOutputID >= rhs.inputDataConnections.size()) {
		throw std::runtime_error(
			"Out of bounds in data connection: there is no input data dock with id " +
			std::to_string(connectionInputID) + " in node with type " + rhs.type->module + ':' +
			rhs.type->name);
	}

	lhs.outputDataConnections[connectionInputID] = {&rhs, connectionOutputID};
	rhs.inputDataConnections[connectionOutputID] = {&lhs, connectionInputID};
}



/// Connects two nodes' exec connections
/// \param lhs The node to the left, the node outputting the connections
/// \param connectionInputID The ID of exec connection in \c lhs to be connected
/// \param rhs The node to the right, that takes in the exec as a parameter
/// \param connectionOutputID The ID of exec input in \c rhs
inline void connectExec(
	NodeInstance& lhs, size_t connectionInputID, NodeInstance& rhs, size_t connectionOutputID)
{
	// make sure the connection exists
	if (connectionInputID >= lhs.outputExecConnections.size()) {
		throw std::runtime_error(
			"Out of bounds in exec connection: there is no output exec dock with id " +
			std::to_string(connectionInputID) + " in node with type " + lhs.type->module + ':' +
			lhs.type->name);
	}
	if (connectionOutputID >= rhs.inputExecConnections.size()) {
		throw std::runtime_error(
			"Out of bounds in exec connection: there is no input exec dock with id " +
			std::to_string(connectionOutputID) + " in node with type " + rhs.type->module + ':' +
			rhs.type->name);
	}

	// connect it!
	lhs.outputExecConnections[connectionInputID] = {&rhs, connectionOutputID};
	rhs.inputExecConnections[connectionOutputID] = {&lhs, connectionOutputID};
}
}

#endif  // CHIG_NODE_INSTANCE_HPP
