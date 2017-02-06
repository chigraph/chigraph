#include "chig/JsonSerializer.hpp"
#include "chig/DataType.hpp"
#include "chig/GraphFunction.hpp"
#include "chig/GraphModule.hpp"
#include "chig/NodeInstance.hpp"
#include "chig/GraphStruct.hpp"

namespace chig {

nlohmann::json graphFunctionToJson(const GraphFunction& func) {
	auto jsonData = nlohmann::json{};

	jsonData["type"] = "function";
	jsonData["name"] = func.name();

	auto& datainputsjson = jsonData["data_inputs"];
	datainputsjson       = nlohmann::json::array();

	for (auto& in : func.dataInputs()) {
		datainputsjson.push_back({{in.name, in.type.qualifiedName()}});
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
		std::string nodeID = nodepair.first;

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
				                           {"output", {conn.first->id(), conn.second}}});
			}
		}

		// add the data outputs
		for (auto conn_id = 0ull; conn_id < node->inputDataConnections.size(); ++conn_id) {
			// if there is actually a connection
			auto& connpair = node->inputDataConnections[conn_id];
			if (connpair.first != nullptr) {
				jsonConnections.push_back({{"type", "data"},
				                           {"input", {connpair.first->id(), connpair.second}},
				                           {"output", {nodeID, conn_id}}});
			}
		}
	}

	return jsonData;
}

nlohmann::json graphModuleToJson(const GraphModule& mod) {
	nlohmann::json data;

	data["name"]         = mod.name();
	data["dependencies"] = mod.dependencies();

	auto& graphsjson = data["graphs"];
	graphsjson       = nlohmann::json::array();
	for (auto& graph : mod.functions()) { graphsjson.push_back(graphFunctionToJson(*graph)); }

	auto& structsJson = data["types"];
	structsJson = nlohmann::json::object();
	for(const auto& str : mod.structs()) {
		structsJson[str->name()] = graphStructToJson(*str);
	}
	
	return data;
}


nlohmann::json graphStructToJson(const GraphStruct& struc) {
	
	nlohmann::json ret = nlohmann::json::array();
	
	for(const auto& elem : struc.types()) {
		ret.push_back({{elem.name, elem.type.qualifiedName()}});
	}
	
	return ret;
	
}

}  // namespace chig
