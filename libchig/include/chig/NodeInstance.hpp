#ifndef CHIG_NODE_INSTANCE_HPP
#define CHIG_NODE_INSTANCE_HPP

#pragma once

#include "chig/NodeType.hpp"

#include <vector>

namespace chig
{
struct NodeInstance {
	/// Construct a nodeinstace with a type location and an ID
	/// \param nodeType The type that the node is
	/// \param posX The X location of the node
	/// \param posY The Y location of the node
	/// \param nodeID The unique ID for the node
	NodeInstance(std::unique_ptr<NodeType> nodeType, float posX, float posY, std::string nodeID);

	NodeInstance(NodeInstance&&) = default;
	NodeInstance(const NodeInstance& other);

	NodeInstance& operator=(NodeInstance&&) = default;
	NodeInstance& operator=(const NodeInstance&);

    NodeType& type() { return *mType; }
    const NodeType& type() const { return *mType; }
    
    float x() const { return mX; }
    float y() const { return mY; }
    
    void setX(float newX) { mX = newX; }
    void setY(float newY) { mY = newY; }
    
    std::string id() const { return mId; }
    
	// connections
	std::vector<std::vector<std::pair<NodeInstance*, size_t>>> inputExecConnections;
	std::vector<std::pair<NodeInstance*, size_t>> inputDataConnections;
	std::vector<std::pair<NodeInstance*, size_t>> outputExecConnections;
	std::vector<std::vector<std::pair<NodeInstance*, size_t>>> outputDataConnections;
    
private:
    
	// general data
	std::unique_ptr<NodeType> mType = nullptr;

	float mX = 0.f;
	float mY = 0.0;


	std::string mId;
};

/// Connects two nodes' data connections
/// \param lhs The node to the left, the node outputting the data
/// \param connectionInputID The ID of data connection in \c lhs to be connected
/// \param rhs The node to the right, that takes in the data as a parameter
/// \param connectionOutputID The ID of data input in \c rhs
/// \return The result
Result connectData(
	NodeInstance& lhs, size_t connectionInputID, NodeInstance& rhs, size_t connectionOutputID);

/// Connects two nodes' exec connections
/// \param lhs The node to the left, the node outputting the connections
/// \param connectionInputID The ID of exec connection in \c lhs to be connected
/// \param rhs The node to the right, that takes in the exec as a parameter
/// \param connectionOutputID The ID of exec input in \c rhs
/// \return The result
Result connectExec(
	NodeInstance& lhs, size_t connectionInputID, NodeInstance& rhs, size_t connectionOutputID);

/// Disconnect a data connection
/// \param lhs The left hand node
/// \param connectionInputID The ID of the data connection to disconnect
/// \param rhs The node that the data port is connected to
/// \return The result
Result disconnectData(NodeInstance& lhs, size_t connectionInputID, NodeInstance& rhs);

/// Disconnect a exec connection
/// \param lhs The left hand node in the connection
/// \param connectionInputID The ID of the connection on lhs
/// \return The result
Result disconnectExec(NodeInstance& lhs, size_t connectionInputID);
}

#endif  // CHIG_NODE_INSTANCE_HPP
