#include "chig/NodeInstance.hpp"

using namespace chig;

NodeInstance::NodeInstance(NodeType* nodeType, float arg_x, float arg_y) :
	type{nodeType},
	x{arg_x},
	y{arg_y} {
	assert(nodeType);
	
	// TODO: multiple exec inputs
	inputExecConnections.resize(1, {nullptr, ~0});
	
	inputDataConnections.resize(nodeType->inputs.size(), {nullptr, ~0});
	
	outputExecConnections.resize(nodeType->numOutputExecs, {nullptr, ~0});
	outputDataConnections.resize(nodeType->outputs.size(), {nullptr, ~0});
}


void NodeInstance::connectExec(NodeInstance* other, unsigned int inputID, unsigned int otherOutputID) {
	
	inputExecConnections[inputID] = {other, otherOutputID};
	other->outputExecConnections[otherOutputID] = {this, inputID};
	
}

void chig::NodeInstance::connectData(chig::NodeInstance* other, unsigned int inputID, unsigned int otherOutputID) {
	
	inputDataConnections[inputID] = {other, otherOutputID};
	other->outputDataConnections[otherOutputID] = {this, inputID};
	
}



