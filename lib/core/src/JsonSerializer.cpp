/// \file JsonSerializer.cpp

#include "chi/JsonSerializer.hpp"
#include "chi/DataType.hpp"
#include "chi/GraphFunction.hpp"
#include "chi/GraphModule.hpp"
#include "chi/GraphStruct.hpp"
#include "chi/NodeInstance.hpp"
#include "chi/NodeType.hpp"

namespace chi {

nlohmann::json graphFunctionToJson(const GraphFunction& func) {
	auto jsonData = nlohmann::json{};

	jsonData["type"]        = "function";
	jsonData["name"]        = func.name();
	jsonData["description"] = func.description();

	auto& datainputsjson = jsonData["data_inputs"];
	datainputsjson       = nlohmann::json::array();

	for (auto& in : func.dataInputs()) {
		datainputsjson.push_back({{in.name, in.type.qualifiedName()}});
	}

	// serialize the local variables
	auto& localsjson = jsonData["local_variables"];
	localsjson       = nlohmann::json::object();
	for (const auto& local : func.localVariables()) {
		localsjson[local.name] = local.type.qualifiedName();
	}

	auto& dataoutputsjson = jsonData["data_outputs"];
	dataoutputsjson       = nlohmann::json::array();

	for (auto& out : func.dataOutputs()) {
		dataoutputsjson.push_back({{out.name, out.type.qualifiedName()}});
	}

	auto& execinputsjson = jsonData["exec_inputs"];
	execinputsjson       = nlohmann::json::array();

	for (auto& in : func.execInputs()) { execinputsjson.push_back(in); }

	auto& execoutputsjson = jsonData["exec_outputs"];
	execoutputsjson       = nlohmann::json::array();

	for (auto& out : func.execOutputs()) { execoutputsjson.push_back(out); }

	// serialize the nodes
	auto& jsonNodes = jsonData["nodes"];
	jsonNodes       = nlohmann::json::object();  // make sure even if it's empty it's an object
	auto& jsonConnections = jsonData["connections"];
	jsonConnections       = nlohmann::json::array();  // make sure even if it's empty it's an aray

	for (const auto& nodepair : func.nodes()) {
		auto&       node   = nodepair.second;
		std::string nodeID = nodepair.first.toString();

		nlohmann::json nodeJson = node->type().toJSON();
		jsonNodes[nodeID]       = {{"type", node->type().qualifiedName()},
                             {"location", {node->x(), node->y()}},
                             {"data", nodeJson}};
		// add its connections. Just out the outputs to avoid duplicates

		// add the exec outputs
		for (auto conn_id = 0ull; conn_id < node->outputExecConnections.size(); ++conn_id) {
			auto& conn = node->outputExecConnections[conn_id];
			// if there is actually a connection
			if (conn.first != nullptr) {
				jsonConnections.push_back({{"type", "exec"},
				                           {"input", {nodeID, conn_id}},
				                           {"output", {conn.first->stringId(), conn.second}}});
			}
		}

		// add the data outputs
		for (auto conn_id = 0ull; conn_id < node->inputDataConnections.size(); ++conn_id) {
			// if there is actually a connection
			auto& connpair = node->inputDataConnections[conn_id];
			if (connpair.first != nullptr) {
				jsonConnections.push_back({{"type", "data"},
				                           {"input", {connpair.first->stringId(), connpair.second}},
				                           {"output", {nodeID, conn_id}}});
			}
		}
	}

	return jsonData;
}

nlohmann::json graphModuleToJson(const GraphModule& mod) {
	nlohmann::json data;

	auto& depsjson = data["dependencies"];
	depsjson       = nlohmann::json::array();
	for (const auto& dep : mod.dependencies()) { depsjson.push_back(dep.generic_string()); }

	auto& graphsjson = data["graphs"];
	graphsjson       = nlohmann::json::array();
	for (auto& graph : mod.functions()) { graphsjson.push_back(graphFunctionToJson(*graph)); }

	auto& structsJson = data["types"];
	structsJson       = nlohmann::json::object();
	for (const auto& str : mod.structs()) { structsJson[str->name()] = graphStructToJson(*str); }

	data["has_c_support"] = mod.cEnabled();

	return data;
}

nlohmann::json graphStructToJson(const GraphStruct& struc) {
	nlohmann::json ret = nlohmann::json::array();

	for (const auto& elem : struc.types()) {
		ret.push_back({{elem.name, elem.type.qualifiedName()}});
	}

	return ret;
}

}  // namespace chi
