#include "chig/NodeInstance.hpp"

using namespace chig;

NodeInstance::NodeInstance(std::unique_ptr<NodeType> nodeType, float arg_x, float arg_y) :
	type{std::move(nodeType)},
	x{arg_x},
	y{arg_y} {
	assert(type);
	
	// TODO: multiple exec inputs
	inputExecConnections.resize(1, {nullptr, ~0});
	
	inputDataConnections.resize(type->inputs.size(), {nullptr, ~0});
	
	outputExecConnections.resize(type->numOutputExecs, {nullptr, ~0});
	outputDataConnections.resize(type->outputs.size(), {nullptr, ~0});
}





