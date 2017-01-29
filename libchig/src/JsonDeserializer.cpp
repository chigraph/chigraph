#include "chig/JsonDeserializer.hpp"

#include "chig/Result.hpp"
#include "chig/GraphModule.hpp"
#include "chig/Context.hpp"
#include "chig/GraphFunction.hpp"
#include "chig/NodeInstance.hpp"

namespace chig {

Result jsonToGraphModule(Context& createInside, const nlohmann::json& input, gsl::cstring_span<> fullName, GraphModule** toFill) {

	Result res;
	
	// create the module
	auto createdModule = createInside.newGraphModule(fullName);
	if(toFill != nullptr) {
		*toFill = createdModule;
	}

	// load dependencies
	{
		auto iter = input.find("dependencies");
		if (iter == input.end()) {
			res.addEntry("E38", "No dependencies element in module", {});
			return res;
		}
		if (!iter->is_array()) {
			res.addEntry("E39", "dependencies element isn't an array", {});
			return res;
		}

		for (const auto& dep : *iter) {
			if (!dep.is_string()) {
				res.addEntry("E40", "dependency isn't a string", {{"Actual Data", dep}});
				continue;
			}
			res += createdModule->addDependency(dep);

			if (!res) { return res; }
		}
	}

	// load graphs
	{
		auto iter = input.find("graphs");
		if (iter == input.end()) {
			res.addEntry("E41", "no graphs element in module", {});
			return res;
		}
		if (!iter->is_array()) {
			res.addEntry("E42", "graph element isn't an array", {{"Actual Data", *iter}});
			return res;
		}
		
		std::vector<GraphFunction*> functions;
		functions.resize(iter->size());
		
		// create forward declarations
		auto id = 0ull;
		for (const auto& graph : *iter) {
			
			res += createGraphFunctionDeclarationFromJson(*createdModule, graph, &functions[id]);			

			++id;
		}
		
		if (!res) {
			return res;
		}
		
		// load the graphs
		id = 0;
		for(const auto& graph : *iter) {
			
			res += jsonToGraphFunction(*functions[id], graph);
			
			++id;
		}
	}
	return res;
}

Result createGraphFunctionDeclarationFromJson(GraphModule& createInside, const nlohmann::json& input, GraphFunction** toFill) {
	Result res;
	
	if (!input.is_object()) {
		res.addEntry("E1", "Graph json isn't a JSON object", {});
		return res;
	}
	// make sure it has a type element
	if (input.find("type") == input.end()) {
		res.addEntry("E2", R"(JSON in graph doesn't have a "type" element)", {});
		return res;
	}
	if (input["type"] != "function") {
		res.addEntry("E3", "JSON in graph doesn't have a function type", {});
		return res;
	}
	// make sure there is a name
	if (input.find("name") == input.end()) {
		res.addEntry("E4", "JSON in graph doesn't have a name parameter", {});
		return res;
	}
	std::string name = input["name"];

	if (input.find("data_inputs") == input.end() || !input["data_inputs"].is_array()) {
		res.addEntry("E43", "JSON in graph doesn't have an data_inputs array", {});
		return res;
	}

	std::vector<std::pair<DataType, std::string>> datainputs;
	for (auto param : input["data_inputs"]) {
		for (auto iter = param.begin(); iter != param.end(); ++iter) {
			std::string qualifiedType = iter.value();
			std::string docString     = iter.key();

			std::string moduleName, name;
			std::tie(moduleName, name) = parseColonPair(qualifiedType);

			DataType ty;
			res += createInside.context().typeFromModule(moduleName, name, &ty);

			if (!res) { return res; }

			datainputs.emplace_back(ty, docString);
		}
	}

	if (input.find("data_outputs") == input.end() || !input["data_outputs"].is_array()) {
		res.addEntry("E44", "JSON in graph doesn't have an data_outputs array", {});
		return res;
	}

	std::vector<std::pair<DataType, std::string>> dataoutputs;
	for (auto param : input["data_outputs"]) {
		for (auto iter = param.begin(); iter != param.end(); ++iter) {
			std::string qualifiedType = iter.value();
			std::string docString     = iter.key();

			std::string moduleName, name;
			std::tie(moduleName, name) = parseColonPair(qualifiedType);

			DataType ty;
			res += createInside.context().typeFromModule(moduleName, name, &ty);

			if (!res) { return res; }

			dataoutputs.emplace_back(ty, docString);
		}
	}

	// get exec I/O
	if (input.find("exec_inputs") == input.end() || !input["exec_inputs"].is_array()) {
		res.addEntry("EUKN", "JSON in graph doesn't have an exec_inputs array", {});
		return res;
	}

	std::vector<std::string> execinputs;
	for (const auto& param : input["exec_inputs"]) {
		std::string name = param;

		execinputs.emplace_back(name);
	}

	if (input.find("exec_outputs") == input.end() || !input["exec_outputs"].is_array()) {
		res.addEntry("EUKN", "JSON in graph doesn't have an data_outputs array", {});
		return res;
	}

	std::vector<std::string> execoutputs;
	for (const auto& param : input["exec_outputs"]) {
		std::string name = param;

		execoutputs.emplace_back(name);
	}

	// construct it
	createInside.createFunction(name, datainputs, dataoutputs, execinputs, execoutputs, toFill);
	
	return res;
}

Result jsonToGraphFunction(GraphFunction& createInside, const nlohmann::json& input) {
	
	Result res;
	
	// read the nodes
	if (input.find("nodes") == input.end() || !input["nodes"].is_object()) {
		res.addEntry("E5", "JSON in graph doesn't have nodes object", {});
		return res;
	}

	for (auto nodeiter = input["nodes"].begin(); nodeiter != input["nodes"].end(); ++nodeiter) {
		auto        node   = nodeiter.value();
		std::string nodeid = nodeiter.key();
		if (node.find("type") == node.end() || !node.find("type")->is_string()) {
			res.addEntry("E6", R"(Node doesn't have a "type" string)", {{"nodeid", nodeid}});
			return res;
		}
		std::string fullType = node["type"];
		std::string moduleName, typeName;
		std::tie(moduleName, typeName) = parseColonPair(fullType);

		if (moduleName.empty() || typeName.empty()) {
			res.addEntry("E7", "Incorrect qualified module name (should be module:type)",
			             {{"nodeid", nodeid}, {"Requested Qualified Name", fullType}});
			return res;
		}

		if (node.find("data") == node.end()) {
			res.addEntry("E9", "Node doens't have a data section", {"nodeid", nodeid});
			return res;
		}

		std::unique_ptr<NodeType> nodeType;
		res += createInside.context().nodeTypeFromModule(moduleName, typeName, node["data"], &nodeType);
		if (!res) { continue; }

		auto testIter = node.find("location");
		if (testIter == node.end()) {
			res.addEntry("E12", "Node doesn't have a location.", {{"nodeid", nodeid}});
			continue;
		}

		// make sure it is the right size
		if (!testIter.value().is_array()) {
			res.addEntry("E10", "Node doesn't have a location that is an array.",
			             {{"nodeid", nodeid}});
			continue;
		}

		if (testIter.value().size() != 2) {
			res.addEntry("E11", "Node doesn't have a location that is an array of size 2.",
			             {{"nodeid", nodeid}});
			continue;
		}

		createInside.insertNode(std::move(nodeType), node["location"][0], node["location"][1], nodeid);
	}

	size_t connID = 0;
	// read the connections
	{
		auto connIter = input.find("connections");
		if (connIter == input.end() || !connIter->is_array()) {
			res.addEntry("E13", "No connections array in function", {});
			return res;
		}
		for (auto& connection : input["connections"]) {
			if (connection.find("type") == connection.end() ||
			    !connection.find("type")->is_string()) {
				res.addEntry("E14", "No type string in connection", {"connectionid", connID});
				continue;
			}
			std::string type   = connection["type"];
			bool        isData = type == "data";
			// it either has to be "input" or "exec"
			if (!isData && type != "exec") {
				res.addEntry("E15", "Unrecognized connection type",
				             {{"connectionid", connID}, {"Found Type", type}});
				continue;
			}

			if (connection.find("input") == connection.end()) {
				res.addEntry("E16", "No input element in connection", {{"connectionid", connID}});
				continue;
			}
			if (!connection.find("input")->is_array() || connection.find("input")->size() != 2 ||
			    !connection.find("input")->operator[](0).is_string() ||
			    !connection.find("input")->operator[](1).is_number_integer()) {
				res.addEntry(
				    "E17",
				    "Incorrect connection input format, must be an array of of a string (node id) "
				    "and int (connection id)",
				    {{"connectionid", connID}, {"Requested Type", *connection.find("input")}});
				continue;
			}
			std::string InputNodeID       = connection["input"][0];
			int         InputConnectionID = connection["input"][1];

			if (connection.find("output") == connection.end()) {
				res.addEntry("E18", "No output element in connection", {{"connectionid", connID}});
				continue;
			}
			if (!connection.find("output")->is_array() || connection.find("output")->size() != 2 ||
			    !connection.find("output")->operator[](0).is_string() ||
			    !connection.find("output")->operator[](1).is_number_integer()) {
				res.addEntry(
				    "E19",
				    "Incorrect connection output format, must be an array of a string (node id) "
				    "and int (connection id)",
				    {{"connectionid", connID}, {"Requested Type", *connection.find("output")}});
				continue;
			}
			std::string OutputNodeID       = connection["output"][0];
			int         OutputConnectionID = connection["output"][1];

			// make sure the nodes exist
			if (createInside.nodes().find(InputNodeID) == createInside.nodes().end()) {
				res.addEntry("E20", "Input node for connection doesn't exist",
				             {{"connectionid", connID}, {"Requested Node", InputNodeID}});
				continue;
			}
			if (createInside.nodes().find(OutputNodeID) == createInside.nodes().end()) {
				res.addEntry("E21", "Output node for connection doesn't exist",
				             {{"connectionid", connID}, {"Requested Node", OutputNodeID}});
				continue;
			}

			// connect
			// these functions do bounds checking, it's okay
			if (isData) {
				res += connectData(*createInside.nodes()[InputNodeID], InputConnectionID, *createInside.nodes()[OutputNodeID],
				                   OutputConnectionID);
			} else {
				res += connectExec(*createInside.nodes()[InputNodeID], InputConnectionID, *createInside.nodes()[OutputNodeID],
				                   OutputConnectionID);
			}

			++connID;
		}
	}
	return res;
}

}
