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

namespace chig {
Result connectData(
	NodeInstance& lhs, size_t connectionInputID, NodeInstance& rhs, size_t connectionOutputID)
{
	Result res;

	// make sure the connection exists
	// the input to the connection is the output to the node
	if (connectionInputID >= lhs.outputDataConnections.size()) {
		res.add_entry("E22", "Output Data connection doesn't exist in node",
			{{"Requested ID", connectionInputID},
				{"Node Type", lhs.type->module + ":" + lhs.type->name}});
	}
	if (connectionOutputID >= rhs.inputDataConnections.size()) {
		res.add_entry("E23", "Input Data connection doesn't exist in node",
			{{"Requested ID", connectionOutputID},
				{"Node Type", rhs.type->module + ":" + rhs.type->name}});
	}
	
	// if there are errors, back out
	if (!res) return res;

	// make sure the connection is of the right type
	if(lhs.type->dataOutputs[connectionInputID].first != rhs.type->dataInputs[connectionOutputID].first) {
		res.add_entry("E24", "Connecting data nodes with different types is invalid", {
			{"Left Hand Type", lhs.type->context->stringifyType(lhs.type->dataOutputs[connectionInputID].first)},
			{"Right Hand Type", lhs.type->context->stringifyType(rhs.type->dataInputs[connectionOutputID].first)}
		});
		return res;
	}

	lhs.outputDataConnections[connectionInputID] = {&rhs, connectionOutputID};
	rhs.inputDataConnections[connectionOutputID] = {&lhs, connectionInputID};

	return res;
}

Result connectExec(
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
