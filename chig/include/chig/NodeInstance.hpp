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
inline Result connectData(
	NodeInstance& lhs, size_t connectionInputID, NodeInstance& rhs, size_t connectionOutputID)
{
	Result res;

	// make sure the connection exists
	// the input to the connection is the output to the node
	if (connectionInputID >= lhs.outputDataConnections.size()) {
		res.add_entry("E22", "Output Data connection doesn't exist in node",
			{{"Requested ID", connectionInputID},
				{"Node Type", lhs.type->module + ":" + lhs.type->name}});
		return res;
	}
	if (connectionOutputID >= rhs.inputDataConnections.size()) {
		res.add_entry("E23", "Input Data connection doesn't exist in node",
			{{"Requested ID", connectionOutputID},
				{"Node Type", rhs.type->module + ":" + rhs.type->name}});
		return res;
	}
	
	// make sure the connection is of the right type
	if(lhs.type->dataOutputs[connectionInputID].first != rhs.type->dataInputs[connectionOutputID].first) {
		res.add_entry("E24", "Connecting data nodes with different types is invalid", {
			{"Left Hand Type", lhs.type->context->stringifyType(lhs.type->dataOutputs[connectionInputID].first)},
			{"Right Hand Type", lhs.type->context->stringifyType(rhs.type->dataInputs[connectionOutputID].first)}
		});
		return res;
	}

	// if there are errors, back out
	if (!res) return res;

	lhs.outputDataConnections[connectionInputID] = {&rhs, connectionOutputID};
	rhs.inputDataConnections[connectionOutputID] = {&lhs, connectionInputID};

	return res;
}

/// Connects two nodes' exec connections
/// \param lhs The node to the left, the node outputting the connections
/// \param connectionInputID The ID of exec connection in \c lhs to be connected
/// \param rhs The node to the right, that takes in the exec as a parameter
/// \param connectionOutputID The ID of exec input in \c rhs
inline Result connectExec(
	NodeInstance& lhs, size_t connectionInputID, NodeInstance& rhs, size_t connectionOutputID)
{
	Result res;

	// make sure the connection exists
	if (connectionInputID >= lhs.outputExecConnections.size()) {
		res.add_entry("E22", "Output exec connection doesn't exist in node",
			{{"Requested ID", connectionInputID},
				{"Node Type", lhs.type->module + ":" + lhs.type->name}});
	}
	if (connectionOutputID >= rhs.inputExecConnections.size()) {
		res.add_entry("E23", "Input exec connection doesn't exist in node",
			{{"Requested ID", connectionInputID},
				{"Node Type", rhs.type->module + ":" + rhs.type->name}});
	}

	if (!res) return res;

	// connect it!
	lhs.outputExecConnections[connectionInputID] = {&rhs, connectionOutputID};
	rhs.inputExecConnections[connectionOutputID] = {&lhs, connectionOutputID};

	return res;
}
}

#endif  // CHIG_NODE_INSTANCE_HPP
