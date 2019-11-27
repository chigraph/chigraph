/// \file chi/NodeInstance.hpp
/// Defines the NodeInstance class and related functions

#ifndef CHI_NODE_INSTANCE_HPP
#define CHI_NODE_INSTANCE_HPP

#pragma once

#include <memory>
#include <vector>

#include "chi/Fwd.hpp"
#include "chi/Support/Uuid.hpp"

namespace chi {
/// An instance of a node
struct NodeInstance {
	/// Construct a nodeinstace with a type location and an ID, usually called from
	/// GraphFunction::insertNode
	/// \param func The function this nodeInstance belongs to
	/// \param nodeType The type that the node is
	/// \param posX The X location of the node
	/// \param posY The Y location of the node
	/// \param nodeID The unique ID for the node
	NodeInstance(GraphFunction* func, std::unique_ptr<NodeType> nodeType, float posX, float posY,
	             Uuid nodeID = Uuid::random());

	/// Destructor
	~NodeInstance();

	/// Move constructor
	NodeInstance(NodeInstance&&) = default;

	/// Copy constructor
	explicit NodeInstance(const NodeInstance& other, Uuid id = Uuid::random());

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
	Uuid id() const { return mId; }

	/// Get the ID as a string
	/// \return String representation of the id
	std::string stringId() const { return id().toString(); }

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
	/// \return The Context
	Context& context() const { return *mContext; }

	/// Get the containing GraphFunction
	/// \return The Function this node belongs to
	GraphFunction& function() const { return *mFunction; }

	/// Get the containing GraphModule
	/// \return The GraphModule this node belongs to
	GraphModule& module() const { return *mGraphModule; }

private:
	std::unique_ptr<NodeType> mType;

	float mX = 0.f;
	float mY = 0.0;

	Uuid mId;

	Context*       mContext;
	GraphFunction* mFunction    = nullptr;
	GraphModule*   mGraphModule = nullptr;
};

/// \name Connection Manipulation
/// \brief Functions for connecting and disconnecting nodes
/// \{

/// Connects two nodes' data connections
/// \param lhs The node to the left, the node outputting the data
/// \param lhsConnID The ID of data connection in \c lhs to be connected
/// \param rhs The node to the right, that takes in the data as a parameter
/// \param rhsConnID The ID of data input in \c rhs
/// \pre `lhs.function() == rhs.connection()`
/// \return The result
Result connectData(NodeInstance& lhs, size_t lhsConnID, NodeInstance& rhs, size_t rhsConnID);

/// Connects two nodes' exec connections
/// \param lhs The node to the left, the node outputting the connections
/// \param lhsConnID The ID of exec connection in \c lhs to be connected
/// \param rhs The node to the right, that takes in the exec as a parameter
/// \param rhsConnID The ID of exec input in \c rhs
/// \pre `lhs.function() == rhs.connection()`
/// \return The result
Result connectExec(NodeInstance& lhs, size_t lhsConnID, NodeInstance& rhs, size_t rhsConnID);

/// Disconnect a data connection
/// \param lhs The left hand node
/// \param lhsConnID The ID of the data connection to disconnect
/// \param rhs The node that the data port is connected to
/// \pre `lhs.function() == rhs.connection()`
/// \return The result
Result disconnectData(NodeInstance& lhs, size_t lhsConnID, NodeInstance& rhs);

/// Disconnect a exec connection
/// \param lhs The left hand node in the connection
/// \param lhsConnID The ID of the connection on lhs
/// \return The result
Result disconnectExec(NodeInstance& lhs, size_t lhsConnID);

/// \}

}  // namespace chi

#endif  // CHI_NODE_INSTANCE_HPP
