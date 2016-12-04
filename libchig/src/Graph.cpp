#include "chig/Graph.hpp"
#include <chig/GraphFunction.hpp>

using namespace chig;

Graph::Graph(Context& con, const nlohmann::json& data, Result& res) : context{&con}
{
	// read the nodes
	if (data.find("nodes") == data.end() || !data["nodes"].is_object()) {
		res.add_entry("E5", "JSON in graph doesn't have nodes object", {});
		return;
	}

	for (auto nodeiter = data["nodes"].begin(); nodeiter != data["nodes"].end(); ++nodeiter) {
		auto node = nodeiter.value();
		std::string nodeid = nodeiter.key();
		if (node.find("type") == node.end() || !node.find("type")->is_string()) {
			res.add_entry("E6", R"(Node doesn't have a "type" string)", {{"nodeid", nodeid}});
			return;
		}
		std::string fullType = node["type"];
		std::string moduleName, typeName;
		std::tie(moduleName, typeName) = parseColonPair(fullType);

		if (moduleName.empty() || typeName.empty()) {
			res.add_entry("E7", "Incorrect qualified module name (should be module:type)",
				{{"nodeid", nodeid}, {"Requested Qualified Name", fullType}});
			return;
		}

		if (node.find("data") == node.end()) {
			res.add_entry("E9", "Node doens't have a data section", {"nodeid", nodeid});
			return;
		}

		std::unique_ptr<NodeType> nodeType;
		res += context->getNodeType(moduleName, typeName, node["data"], &nodeType);
		if (!res) {
			continue;
		}

		auto testIter = node.find("location");
		if (testIter != node.end()) {
			// make sure it is the right size
			if (!testIter.value().is_array()) {
				res.add_entry(
					"E10", "Node doesn't have a location that is an array.", {{"nodeid", nodeid}});
				continue;
			}

			if (testIter.value().size() != 2) {
				res.add_entry("E11", "Node doesn't have a location that is an array of size 2.",
					{{"nodeid", nodeid}});
				continue;
			}
		} else {
			res.add_entry("E12", "Node doesn't have a location.", {{"nodeid", nodeid}});
			continue;
		}

		insertNode(std::move(nodeType), node["location"][0], node["location"][1], nodeid);
	}

	size_t connID = 0;
	// read the connections
	{
		auto connIter = data.find("connections");
		if (connIter == data.end() || !connIter->is_array()) {
			res.add_entry("E13", "No connections array in function", {});
			return;
		}
		for (auto& connection : data["connections"]) {
			if (connection.find("type") == connection.end() ||
				!connection.find("type")->is_string()) {
				res.add_entry("E14", "No type string in connection", {"connectionid", connID});
				continue;
			}
			std::string type = connection["type"];
			bool isData = type == "data";
			// it either has to be "data" or "exec"
			if (!isData && type != "exec") {
				res.add_entry("E15", "Unrecognized connection type",
					{{"connectionid", connID}, {"Found Type", type}});
				continue;
			}

			if (connection.find("input") == connection.end()) {
				res.add_entry("E16", "No input element in connection", {{"connectionid", connID}});
				continue;
			}
			if (!connection.find("input")->is_array() || connection.find("input")->size() != 2 ||
				!connection.find("input")->operator[](0).is_string() ||
				!connection.find("input")->operator[](1).is_number_integer()) {
				res.add_entry("E17",
					"Incorrect connection input format, must be an array of of a string (node id) "
					"and int (connection id)",
					{{"connectionid", connID}, {"Requested Type", *connection.find("input")}});
				continue;
			}
			std::string InputNodeID = connection["input"][0];
			int InputConnectionID = connection["input"][1];

			if (connection.find("output") == connection.end()) {
				res.add_entry("E18", "No output element in connection", {{"connectionid", connID}});
				continue;
			}
			if (!connection.find("output")->is_array() || connection.find("output")->size() != 2 ||
				!connection.find("output")->operator[](0).is_string() ||
				!connection.find("output")->operator[](1).is_number_integer()) {
				res.add_entry("E19",
					"Incorrect connection output format, must be an array of a string (node id) "
					"and int (connection id)",
					{{"connectionid", connID}, {"Requested Type", *connection.find("output")}});
				continue;
			}
			std::string OutputNodeID = connection["output"][0];
			int OutputConnectionID = connection["output"][1];

			// make sure the nodes exist
			if (nodes.find(InputNodeID) == nodes.end()) {
				res.add_entry("E20", "Input node for connection doesn't exist",
					{{"connectionid", connID}, {"Requested Node", InputNodeID}});
				continue;
			}
			if (nodes.find(OutputNodeID) == nodes.end()) {
				res.add_entry("E21", "Output node for connection doesn't exist",
					{{"connectionid", connID}, {"Requested Node", OutputNodeID}});
				continue;
			}

			// connect
			// these functions do bounds checking, it's okay
			if (isData) {
				res += connectData(*nodes[InputNodeID], InputConnectionID, *nodes[OutputNodeID],
					OutputConnectionID);
			} else {
				res += connectExec(*nodes[InputNodeID], InputConnectionID, *nodes[OutputNodeID],
					OutputConnectionID);
			}

			++connID;
		}
	}
}

Result Graph::toJson(nlohmann::json* toFill) const
{
	auto& jsonData = *toFill;

	// serialize the nodes
	auto& jsonNodes = jsonData["nodes"];
	jsonNodes = nlohmann::json::object();  // make sure even if it's empty it's an object
	auto& jsonConnections = jsonData["connections"];
	jsonConnections = nlohmann::json::array();  // make sure even if it's empty it's an aray

	for (const auto& nodepair : nodes) {
		auto& node = nodepair.second;
		std::string nodeID = nodepair.first;

		nlohmann::json nodeJson = node->type->toJSON();
		jsonNodes[nodeID] = {{"type", node->type->getQualifiedName()},
			{"location", {node->x, node->y}}, {"data", nodeJson}};
		// add its connections. Just out the outputs to avoid duplicates

		// add the exec outputs
		for (auto conn_id = 0ull; conn_id < node->outputExecConnections.size(); ++conn_id) {
			auto& conn = node->outputExecConnections[conn_id];
			// if there is actually a connection
			if (conn.first != nullptr) {
				jsonConnections.push_back({{"type", "exec"}, {"input", {nodeID, conn_id}},
					{"output", {conn.first->id, conn.second}}});
			}
		}

		// add the data outputs
		for (auto conn_id = 0ull; conn_id < node->inputDataConnections.size(); ++conn_id) {
			// if there is actually a connection
			auto& connpair = node->inputDataConnections[conn_id];
			if (connpair.first != nullptr) {
				jsonConnections.push_back(
					{{"type", "data"}, {"input", {connpair.first->id, connpair.second}},
						{"output", {nodeID, conn_id}}});
			}
		}
	}

	return {};
}

NodeInstance* Graph::insertNode(
	std::unique_ptr<NodeType> type, float x, float y, const std::string& id)
{
	auto ptr = std::make_unique<NodeInstance>(std::move(type), x, y, id);

	auto emplaced = nodes.emplace(id, std::move(ptr)).first;

	return emplaced->second.get();
}

std::vector<NodeInstance*> Graph::getNodesWithType(const char* module, const char* name) const
	noexcept
{
	auto typeFinder = [&](auto& pair) {
		return pair.second->type->module->name == module && pair.second->type->name == name;
	};

	std::vector<NodeInstance*> ret;
	auto iter = std::find_if(nodes.begin(), nodes.end(), typeFinder);
	while (iter != nodes.end()) {
		ret.emplace_back(iter->second.get());

		std::advance(iter, 1);  // don't process the same one twice!
		iter = std::find_if(iter, nodes.end(), typeFinder);
	}

	return ret;
}
