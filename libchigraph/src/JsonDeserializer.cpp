#include "chi/JsonDeserializer.hpp"
#include "chi/Context.hpp"
#include "chi/GraphFunction.hpp"
#include "chi/GraphModule.hpp"
#include "chi/GraphStruct.hpp"
#include "chi/NodeInstance.hpp"
#include "chi/NodeType.hpp"
#include "chi/Result.hpp"

namespace chi {

Result jsonToGraphModule(Context& createInside, const nlohmann::json& input,
                         const boost::filesystem::path& fullName, GraphModule** toFill) {
	Result res;

	// create the module
	auto createdModule = createInside.newGraphModule(fullName);
	if (toFill != nullptr) { *toFill = createdModule; }

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

	// load types
	{
		auto iter = input.find("types");
		if (iter == input.end() || !iter->is_object()) {
			res.addEntry("EUKN", "No types object in module", {{}});
			return res;
		}

		// declare them
		for (auto tyIter = iter->begin(); tyIter != iter->end(); ++tyIter) {
			createdModule->getOrCreateStruct(tyIter.key());
		}
		// load them
		for (auto tyIter = iter->begin(); tyIter != iter->end(); ++tyIter) {
			res += jsonToGraphStruct(*createdModule, tyIter.key(), tyIter.value());
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

		if (!res) { return res; }

		// load the graphs
		id = 0;
		for (const auto& graph : *iter) {
			res += jsonToGraphFunction(*functions[id], graph);

			++id;
		}
	}

	return res;
}

Result createGraphFunctionDeclarationFromJson(GraphModule&          createInside,
                                              const nlohmann::json& input, GraphFunction** toFill) {
	Result res;

	if (!input.is_object()) {
		res.addEntry("E1", "Graph json isn't a JSON object", {});
		return res;
	}
	// make sure it has a type element
	if (input.find("type") == input.end() || !input["type"].is_string()) {
		res.addEntry("E2", R"(JSON in graph doesn't have a "type" element)",
		             {{"Module Name", createInside.fullName()}});
		return res;
	}
	if (input["type"] != "function") {
		res.addEntry("E3", "JSON in graph doesn't have a function type",
		             {{"Module Name", createInside.fullName()}});
		return res;
	}
	// make sure there is a name
	if (input.find("name") == input.end() || !input["name"].is_string()) {
		res.addEntry("E4", "JSON in graph doesn't have a name parameter",
		             {{"Module Name", createInside.fullName()}});
		return res;
	}
	std::string name = input["name"];

	// load the description
	if (input.find("description") == input.end() || !input["description"].is_string()) {
		res.addEntry("E50", "JSON in graph doesn't have a description string",
		             {{"Function Name", name}, {"Module Name", createInside.fullName()}});
		return res;
	}
	std::string description = input["description"];

	if (input.find("data_inputs") == input.end() || !input["data_inputs"].is_array()) {
		res.addEntry("E43", "JSON in graph doesn't have an data_inputs array", {});
		return res;
	}

	std::vector<NamedDataType> datainputs;
	for (auto param : input["data_inputs"]) {
		std::string docString, qualifiedType;
		std::tie(docString, qualifiedType) = parseObjectPair(param);

		std::string moduleName, name;
		std::tie(moduleName, name) = parseColonPair(qualifiedType);

		DataType ty;
		res += createInside.context().typeFromModule(moduleName, name, &ty);

		if (!res) { return res; }

		datainputs.emplace_back(docString, ty);
	}

	if (input.find("data_outputs") == input.end() || !input["data_outputs"].is_array()) {
		res.addEntry("E44", "JSON in graph doesn't have an data_outputs array", {});
		return res;
	}

	std::vector<NamedDataType> dataoutputs;
	for (auto param : input["data_outputs"]) {
		std::string docString, qualifiedType;
		std::tie(docString, qualifiedType) = parseObjectPair(param);

		std::string moduleName, name;
		std::tie(moduleName, name) = parseColonPair(qualifiedType);

		DataType ty;
		res += createInside.context().typeFromModule(moduleName, name, &ty);

		if (!res) { return res; }

		dataoutputs.emplace_back(docString, ty);
	}

	// get exec I/O
	if (input.find("exec_inputs") == input.end() || !input["exec_inputs"].is_array()) {
		res.addEntry("E48", "JSON in graph doesn't have an exec_inputs array", {});
		return res;
	}

	std::vector<std::string> execinputs;
	for (const auto& param : input["exec_inputs"]) {
		std::string name = param;

		execinputs.emplace_back(name);
	}

	if (input.find("exec_outputs") == input.end() || !input["exec_outputs"].is_array()) {
		res.addEntry("E49", "JSON in graph doesn't have an data_outputs array", {});
		return res;
	}

	std::vector<std::string> execoutputs;
	for (const auto& param : input["exec_outputs"]) {
		std::string name = param;

		execoutputs.emplace_back(name);
	}

	// construct it
	auto created =
	    createInside.getOrCreateFunction(name, datainputs, dataoutputs, execinputs, execoutputs);
	created->setDescription(std::move(description));
	if (toFill != nullptr) { *toFill = created; }

	return res;
}

Result jsonToGraphFunction(GraphFunction& createInside, const nlohmann::json& input) {
	Result res;

	// read the local variables
	if (input.find("local_variables") == input.end() || !input["local_variables"].is_object()) {
		res.addEntry("E45", "JSON in graph doesn't have a local_variables object", {});

		return res;
	}

	for (auto localiter = input["local_variables"].begin();
	     localiter != input["local_variables"].end(); ++localiter) {
		std::string localName = localiter.key();

		if (!localiter.value().is_string()) {
			res.addEntry("E46", "Local variable vaue in json wasn't a string",
			             {{"Given local variable json", localiter.value()}});

			continue;
		}

		// parse the type names
		std::string qualifiedType = localiter.value();

		std::string moduleName, typeName;
		std::tie(moduleName, typeName) = parseColonPair(qualifiedType);

		DataType ty;
		res += createInside.context().typeFromModule(moduleName, typeName, &ty);

		if (!res) { continue; }

		createInside.getOrCreateLocalVariable(localName, ty);
	}

	// read the nodes
	if (input.find("nodes") == input.end() || !input["nodes"].is_object()) {
		res.addEntry("E5", "JSON in graph doesn't have nodes object", {});
		return res;
	}

	for (auto nodeiter = input["nodes"].begin(); nodeiter != input["nodes"].end(); ++nodeiter) {
		auto        node   = nodeiter.value();
		std::string nodeid = nodeiter.key();
		if (node.find("type") == node.end() || !node.find("type")->is_string()) {
			res.addEntry("E6", R"(Node doesn't have a "type" string)", {{"Node ID", nodeid}});
			return res;
		}
		std::string fullType = node["type"];
		std::string moduleName, typeName;
		std::tie(moduleName, typeName) = parseColonPair(fullType);

		if (moduleName.empty() || typeName.empty()) {
			res.addEntry("E7", "Incorrect qualified module name (should be module:type)",
			             {{"Node ID", nodeid}, {"Requested Qualified Name", fullType}});
			return res;
		}

		if (node.find("data") == node.end()) {
			res.addEntry("E9", "Node doens't have a data section", {"Node ID", nodeid});
			return res;
		}

		std::unique_ptr<NodeType> nodeType;
		res += createInside.context().nodeTypeFromModule(moduleName, typeName, node["data"],
		                                                 &nodeType);
		if (!res) { continue; }

		auto testIter = node.find("location");
		if (testIter == node.end()) {
			res.addEntry("E12", "Node doesn't have a location.", {{"Node ID", nodeid}});
			continue;
		}

		// make sure it is the right size
		if (!testIter.value().is_array()) {
			res.addEntry("E10", "Node doesn't have a location that is an array.",
			             {{"Node ID", nodeid}});
			continue;
		}

		if (testIter.value().size() != 2) {
			res.addEntry("E11", "Node doesn't have a location that is an array of size 2.",
			             {{"Node ID", nodeid}});
			continue;
		}

		try {
			auto uuidNodeID = boost::uuids::string_generator()(nodeid);

			createInside.insertNode(std::move(nodeType), node["location"][0], node["location"][1],
			                        uuidNodeID);
		} catch (std::exception& e) {
			res.addEntry("E51", "Invalid UUID string", {{"string", nodeid}});
		}
	}

	// read the connections
	{
		auto connIter = input.find("connections");
		if (connIter == input.end() || !connIter->is_array()) {
			res.addEntry("E13", "No connections array in function", {});
			return res;
		}

		auto connID = 0ull;
		for (auto& connection : input["connections"]) {
			if (connection.find("type") == connection.end() ||
			    !connection.find("type")->is_string()) {
				res.addEntry("E14", "No type string in connection", {"connectionid", connID});

				++connID;
				continue;
			}
			std::string type   = connection["type"];
			bool        isData = type == "data";
			// it either has to be "input" or "exec"
			if (!isData && type != "exec") {
				res.addEntry("E15", "Unrecognized connection type",
				             {{"connectionid", connID}, {"Found Type", type}});

				++connID;
				continue;
			}

			if (connection.find("input") == connection.end()) {
				res.addEntry("E16", "No input element in connection", {{"connectionid", connID}});

				++connID;
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

				++connID;
				continue;
			}
			std::string InputNodeID = connection["input"][0];

			boost::uuids::uuid InputNodeIDUUID;
			try {
				InputNodeIDUUID = boost::uuids::string_generator()(InputNodeID);
			} catch (std::exception&) {
				res.addEntry("EUKN", "Invalid UUID string in connection",
				             {{"string", InputNodeID}});

				++connID;
				continue;
			}
			int InputConnectionID = connection["input"][1];

			if (connection.find("output") == connection.end()) {
				res.addEntry("E18", "No output element in connection", {{"connectionid", connID}});

				++connID;
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
				++connID;
				continue;
			}
			std::string OutputNodeID = connection["output"][0];

			boost::uuids::uuid OutputNodeIDUUID;
			try {
				OutputNodeIDUUID = boost::uuids::string_generator()(OutputNodeID);
			} catch (std::exception&) {
				res.addEntry("EUKN", "Invalid UUID string in connection",
				             {{"string", OutputNodeID}});

				++connID;
				continue;
			}
			int OutputConnectionID = connection["output"][1];

			// make sure the nodes exist
			if (createInside.nodes().find(InputNodeIDUUID) == createInside.nodes().end()) {
				res.addEntry("E20", "Input node for connection doesn't exist",
				             {{"connectionid", connID}, {"Requested Node", InputNodeID}});
				++connID;
				continue;
			}
			if (createInside.nodes().find(OutputNodeIDUUID) == createInside.nodes().end()) {
				res.addEntry("E21", "Output node for connection doesn't exist",
				             {{"connectionid", connID}, {"Requested Node", OutputNodeID}});
				++connID;
				continue;
			}

			// connect
			// these functions do bounds checking, it's okay
			if (isData) {
				res += connectData(*createInside.nodes()[InputNodeIDUUID], InputConnectionID,
				                   *createInside.nodes()[OutputNodeIDUUID], OutputConnectionID);
			} else {
				res += connectExec(*createInside.nodes()[InputNodeIDUUID], InputConnectionID,
				                   *createInside.nodes()[OutputNodeIDUUID], OutputConnectionID);
			}

			++connID;
		}
	}
	return res;
}

Result jsonToGraphStruct(GraphModule& mod, boost::string_view name, const nlohmann::json& input,
                         GraphStruct** toFill) {
	Result res;

	if (!input.is_array()) {
		res.addEntry("EUKN", "Graph Struct json has to be an array", {{"Given JSON", input}});
		return res;
	}

	auto createdStruct = mod.getOrCreateStruct(name.to_string());
	if (toFill != nullptr) { *toFill = createdStruct; }

	for (const auto& str : input) {
		if (!str.is_object()) {
			res.addEntry("EUKN", "Graph Struct entry must be an object", {{"Given JSON", str}});
			continue;
		}

		if (str.size() != 1) {
			res.addEntry("EUKN", "Graph Struct entry must have size of 1", {{"Size", str.size()}});
			continue;
		}

		std::string docstring, qualifiedType;
		std::tie(docstring, qualifiedType) = parseObjectPair(str);

		// parse the type
		std::string typeModuleName, typeName;
		std::tie(typeModuleName, typeName) = parseColonPair(qualifiedType);

		DataType ty;
		res += mod.context().typeFromModule(typeModuleName, typeName, &ty);

		if (!res) { continue; }

		createdStruct->addType(ty, docstring, createdStruct->types().size());
	}

	return res;
}

std::pair<std::string, std::string> parseObjectPair(const nlohmann::json& object) {
	if (!object.is_object()) { return {}; }

	auto iter = object.begin();
	if (iter == object.end()) { return {}; }

	std::string key = iter.key();
	std::string val = iter.value();

	// make sure it's the only element
	++iter;
	if (iter != object.end()) { return {}; }

	return {key, val};
}
}  // namespace chi
