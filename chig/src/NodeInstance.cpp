#include "chig/NodeInstance.hpp"

using namespace chig;

NodeInstance::NodeInstance(std::unique_ptr<NodeType> nodeType, float arg_x, float arg_y)
	: type{std::move(nodeType)}, x{arg_x}, y{arg_y}
{
	assert(type);

	inputDataConnections.resize(type->dataInputs.size(), {nullptr, ~0});
	outputDataConnections.resize(type->dataOutputs.size(), {nullptr, ~0});

	inputExecConnections.resize(type->execInputs.size(), {nullptr, ~0});
	outputExecConnections.resize(type->execOutputs.size(), {nullptr, ~0});
}
