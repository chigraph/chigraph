#ifndef CHIG_NODE_INSTANCE_HPP
#define CHIG_NODE_INSTANCE_HPP

#pragma once

#include "chig/NodeType.hpp"

#include <vector>

namespace chig {

struct NodeInstance {
	
	NodeInstance(NodeType* nodeType);
	
	NodeInstance(const NodeInstance&) = default;
	NodeInstance(NodeInstance&&) = default;
	
	// these functions are called when connecting some node to the inputs. 
	void connectExec(NodeInstance* other, unsigned int inputID, unsigned int otherOutputID);
	void connectData(NodeInstance* other, unsigned int inputID, unsigned int otherOutputID);
	
	// general data
	NodeType* type = nullptr;
	
	float x = 0.f;
	float y = 0.0;
	
	// connections
	std::vector<std::pair<NodeInstance*, unsigned int>> inputExecConnections;
	std::vector<std::pair<NodeInstance*, unsigned int>> inputDataConnections;
	std::vector<std::pair<NodeInstance*, unsigned int>> outputExecConnections;
	std::vector<std::pair<NodeInstance*, unsigned int>> outputDataConnections;
};

}

#endif // CHIG_NODE_INSTANCE_HPP
