#ifndef CHIG_NODE_INSTANCE_HPP
#define CHIG_NODE_INSTANCE_HPP

#pragma once

#include "chig/NodeType.hpp"

#include <vector>

namespace chig {
/// An instance of a node
struct NodeInstance {
	/// Construct a nodeinstace with a type location and an ID, usually called from
	/// GraphFunction::insertNode
	/// \param nodeType The type that the node is
	/// \param posX The X location of the node
	/// \param posY The Y location of the node
	/// \param nodeID The unique ID for the node
	NodeInstance(GraphFunction* func, std::unique_ptr<NodeType> nodeType, float posX, float posY,
				 gsl::cstring_span<> nodeID);

	/// Move constructor
	NodeInstance(NodeInstance&&) = default;

	/// Copy constructor
	NodeInstance(const NodeInstance& other, std::string id);

	/// Move assignment operator
	NodeInstance& operator=(NodeInstance&&) = default;

	/// Copy assignment operator
	NodeInstance& operator=(const NodeInstance&);

	/// Set the type of the node instance
	/// \param newType The new type
	void setType(std::unique_ptr<NodeType> newType);

	/// Get the type of the instance
	/// \return The type
	NodeType& type() { return *mType; }
	/// Get the type of the instance, const version
	/// \return The type
	const NodeType& type() const { return *mType; }
	/// Get the X location of the instance
	/// \return The X coordinate
	float x() const { return mX; }
	/// Get the Y location of the instance
	/// \return The Y coordinate
	float y() const { return mY; }
	/// Set the X location of the instance
	/// \param newX The new X coordinate
	void setX(float newX) { mX = newX; }
	/// Set the Y location of the instance
	/// \param newY The new Y coordinate
	void setY(float newY) { mY = newY; }
	/// Get the ID of the instance, unique to the graph
	/// \return The ID
	std::string id() const { return mId; }
	// connections

	// TODO: better documentation here and OOify
	/// The connections that lead to this node, exec
	std::vector<std::vector<std::pair<NodeInstance*, size_t>>> inputExecConnections;

	/// The connections that go into this node, data
	std::vector<std::pair<NodeInstance*, size_t>> inputDataConnections;

	/// The connections that go out of this node, exec
	std::vector<std::pair<NodeInstance*, size_t>> outputExecConnections;

	/// The connections that lead out of this node, data
	std::vector<std::vector<std::pair<NodeInstance*, size_t>>> outputDataConnections;

	/// Get the containing Context object
	Context& context() const { return *mContext; }

	/// Get the containing GraphFunction
	GraphFunction& function() const { return *mFunction; }

private:
	std::unique_ptr<NodeType> mType = nullptr;

	float mX = 0.f;
	float mY = 0.0;

	std::string mId;

	Context*	   mContext;
	GraphFunction* mFunction;
};

/// Connects two nodes' data connections
/// \param lhs The node to the left, the node outputting the data
/// \param connectionInputID The ID of data connection in \c lhs to be connected
/// \param rhs The node to the right, that takes in the data as a parameter
/// \param connectionOutputID The ID of data input in \c rhs
/// \return The result
Result connectData(NodeInstance& lhs, size_t connectionInputID, NodeInstance& rhs,
				   size_t connectionOutputID);

/// Connects two nodes' exec connections
/// \param lhs The node to the left, the node outputting the connections
/// \param connectionInputID The ID of exec connection in \c lhs to be connected
/// \param rhs The node to the right, that takes in the exec as a parameter
/// \param connectionOutputID The ID of exec input in \c rhs
/// \return The result
Result connectExec(NodeInstance& lhs, size_t connectionInputID, NodeInstance& rhs,
				   size_t connectionOutputID);

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
}  // namespace chig

#endif  // CHIG_NODE_INSTANCE_HPP
