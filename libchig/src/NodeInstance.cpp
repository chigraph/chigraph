#include "chig/NodeInstance.hpp"

using namespace chig;

NodeInstance::NodeInstance(
	std::unique_ptr<NodeType> nodeType, float arg_x, float arg_y, std::string id_)
	: type{std::move(nodeType)}, x{arg_x}, y{arg_y}, id{std::move(id_)}
{
	assert(type);

	inputDataConnections.resize(type->dataInputs.size(), {nullptr, ~0});
	outputDataConnections.resize(type->dataOutputs.size(), {});

	inputExecConnections.resize(type->execInputs.size(), {});
	outputExecConnections.resize(type->execOutputs.size(), {nullptr, ~0});
}

namespace chig
{
Result connectData(
	NodeInstance& lhs, size_t connectionInputID, NodeInstance& rhs, size_t connectionOutputID)
{
	Result res;

	// make sure the connection exists
	// the input to the connection is the output to the node
	if (connectionInputID >= lhs.outputDataConnections.size()) {
		auto dataOutputs = nlohmann::json::array();
		for (auto& output : rhs.type->dataOutputs) {
			dataOutputs.push_back(
				{{output.second, lhs.type->context->stringifyType(output.first)}});
		}

		res.add_entry("E22", "Output Data connection doesn't exist in node",
			{{"Requested ID", connectionInputID},
				{"Node Type", lhs.type->module + ":" + lhs.type->name},
				{"Node JSON", rhs.type->toJSON()}, {"Node Output Data Connections", dataOutputs}});
	}
	if (connectionOutputID >= rhs.inputDataConnections.size()) {
		auto dataInputs = nlohmann::json::array();
		for (auto& output : rhs.type->dataInputs) {
			dataInputs.push_back({{output.second, lhs.type->context->stringifyType(output.first)}});
		}

		res.add_entry("E23", "Input Data connection doesn't exist in node",
			{{"Requested ID", connectionOutputID},
				{"Node Type", rhs.type->module + ":" + rhs.type->name},
				{"Node JSON", rhs.type->toJSON()}, {"Node Input Data Connections", dataInputs}});
	}

	// if there are errors, back out
	if (!res) return res;

	// make sure the connection is of the right type
	if (lhs.type->dataOutputs[connectionInputID].first !=
		rhs.type->dataInputs[connectionOutputID].first) {
		res.add_entry("E24", "Connecting data nodes with different types is invalid",
			{{"Left Hand Type",
				 lhs.type->context->stringifyType(lhs.type->dataOutputs[connectionInputID].first)},
				{"Right Hand Type", lhs.type->context->stringifyType(
										rhs.type->dataInputs[connectionOutputID].first)},
				{"Left Node JSON", rhs.type->toJSON()}, {"Right Node JSON", rhs.type->toJSON()}});
		return res;
	}

	lhs.outputDataConnections[connectionInputID].emplace_back(&rhs, connectionOutputID);
	rhs.inputDataConnections[connectionOutputID] = {&lhs, connectionInputID};

	return res;
}

Result connectExec(
	NodeInstance& lhs, size_t connectionInputID, NodeInstance& rhs, size_t connectionOutputID)
{
	Result res;

	// make sure the connection exists
	if (connectionInputID >= lhs.outputExecConnections.size()) {
		auto execOutputs = nlohmann::json::array();
		for (auto& output : rhs.type->execOutputs) {
			execOutputs.push_back(output);
		}

		res.add_entry("E22", "Output exec connection doesn't exist in node",
			{{"Requested ID", connectionInputID},
				{"Node Type", lhs.type->module + ":" + lhs.type->name},
				{"Node Output Exec Connections", execOutputs}});
	}
	if (connectionOutputID >= rhs.inputExecConnections.size()) {
		auto execInputs = nlohmann::json::array();
		for (auto& output : rhs.type->execInputs) {
			execInputs.push_back(output);
		}

		res.add_entry("E23", "Input exec connection doesn't exist in node",
			{{"Requested ID", connectionInputID},
				{"Node Type", rhs.type->module + ":" + rhs.type->name},
				{"Node Input Exec Connections", execInputs}

			});
	}

	if (!res) return res;

	// connect it!
	lhs.outputExecConnections[connectionInputID] = {&rhs, connectionOutputID};
	rhs.inputExecConnections[connectionOutputID].emplace_back(&lhs, connectionOutputID);

	return res;
}
}
