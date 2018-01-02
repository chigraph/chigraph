/// \file FunctionValidator.cpp

#include "FunctionValidator.hpp"
#include "DataType.hpp"
#include "GraphFunction.hpp"
#include "GraphModule.hpp"
#include "NodeInstance.hpp"
#include "NodeType.hpp"

#include "chi/Support/Result.hpp"

#include <unordered_map>

namespace chi {

Result validateFunction(const GraphFunction& func) {
	Result res;

	res += validateFunctionConnectionsAreTwoWay(func);
	res += validateFunctionNodeInputs(func);
	res += validateFunctionExecOutputs(func);
	res += validateFunctionEntryType(func);
	res += validateFunctionExitTypes(func);

	if (func.name() == "main" && func.module().shortName() == "main") {
		res += validateFunctionMainSignature(func);
	}

	return res;
}

Result validateFunctionConnectionsAreTwoWay(const GraphFunction& func) {
	Result res;

	// make sure they all get the context
	auto funcCtx =
	    res.addScopedContext({{"function", func.name()}, {"module", func.module().fullName()}});

	// make sure all connections connect back
	for (const auto& node : func.nodes()) {
		// go through input data
		auto id = 0ull;
		for (const auto& conn : node.second->inputDataConnections) {
			if (conn.first == nullptr) {
				res.addEntry("EUKN", "Node is missing an input data connection",
				             {{"Node ID", node.second->stringId()},
				              {"nodetype", node.second->type().qualifiedName()},
				              {"requested id", id}});
				++id;
				continue;
			}

			// make sure it connects  back
			bool connectsBack = false;
			for (const auto& remoteConn : conn.first->outputDataConnections[conn.second]) {
				if (remoteConn.first == node.second.get() && remoteConn.second == id) {
					connectsBack = true;
					break;
				}
			}
			if (!connectsBack) {
				res.addEntry("EUKN", "Data connection doesn't connect back",
				             {{"Left Node", conn.first->stringId()},
				              {"Right Node", node.second->stringId()},
				              {"Right input ID", id}});
			}
			++id;
		}

		// output data
		id = 0ull;
		for (const auto& outputDataSlot : node.second->outputDataConnections) {
			// this connection type can make multiple connections, so two for loops are needed
			for (const auto& connection : outputDataSlot) {
				assert(connection.first != nullptr);

				if (connection.first->inputDataConnections.size() <= connection.second) {
					res.addEntry("EUKN", "Input data port not found in node",
					             {{"Node ID", connection.first->stringId()},
					              {"requested id", connection.second}});
					continue;
				}

				auto& remoteConn = connection.first->inputDataConnections[connection.second];
				if (remoteConn.first != node.second.get() || remoteConn.second != id) {
					res.addEntry("EUKN", "Data connection doesn't connect back",
					             {{"Left Node", node.second->stringId()},
					              {"Right Node", connection.first->stringId()},
					              {"Right input ID", connection.second}});
				}
			}
			++id;
		}

		// input exec
		id = 0ull;
		for (const auto& inputExecSlot : node.second->inputExecConnections) {
			// this connection type can make multiple connections, so two for loops are needed
			for (const auto& connection : inputExecSlot) {
				auto& remoteConn = connection.first->outputExecConnections[connection.second];
				if (remoteConn.first != node.second.get() || remoteConn.second != id) {
					res.addEntry("EUKN", "Exec connection doesn't connect back",
					             {{"Left Node", connection.first->stringId()},
					              {"Left Node Type", connection.first->type().qualifiedName()},
					              {"Right Node", node.second->stringId()},
					              {"Right Node Type", node.second->type().qualifiedName()},
					              {"Left output ID", connection.second}});
				}
			}
			++id;
		}

		// output exec
		id = 0ull;
		for (const auto& connection : node.second->outputExecConnections) {
			bool connectsBack = false;

			if (connection.first == nullptr) {
				++id;
				continue;
			}
			for (const auto& remoteConnection :
			     connection.first->inputExecConnections[connection.second]) {
				if (remoteConnection.second == id && remoteConnection.first == node.second.get()) {
					connectsBack = true;
					break;
				}
			}

			if (!connectsBack) {
				res.addEntry("EUKN", "Exec connection doesn't connect back",
				             {{"Left Node", node.second->stringId()},
				              {"Left Node Type", node.second->type().qualifiedName()},
				              {"Right Node", connection.first->stringId()},
				              {"Right Node Type", connection.first->type().qualifiedName()},
				              {"Left output ID", id}});
			}

			++id;
		}
	}

	// make sure all entires have the right type

	return res;
}

namespace {

/// \internal
/// alreadyCalled stores the nodes that have been called at this node, and the inputExecIds it has
/// been called at, to avoid infinite loops
Result validatePath(
    const NodeInstance& inst, int                                            inExecId,
    std::unordered_map<const NodeInstance*, std::vector<int> /*in Exec id*/> alreadyCalled) {
	Result res;

	// if we've already been here, then return, it's a loop
	{
		auto iter = alreadyCalled.find(&inst);
		if (iter != alreadyCalled.end() &&
		    std::find(iter->second.begin(), iter->second.end(), inExecId) != iter->second.end()) {
			return res;
		}
	}

	// make sure the inputs are already in processed
	auto id = 0ull;
	for (const auto& conn : inst.inputDataConnections) {
		if (conn.first == nullptr) {
			res.addEntry("EUKN", "Node is missing an input data connection",
			             {{"Node ID", inst.stringId()},
			              {"dataid", id},
			              {"nodetype", inst.type().qualifiedName()}});
			++id;
			continue;
		}

		if (!conn.first->type().pure() && alreadyCalled.find(conn.first) == alreadyCalled.end()) {
			res.addEntry("EUKN", "Node that accepts data from another node is called first",
			             {{"Node ID", inst.stringId()}, {"othernodeid", conn.first->stringId()}});
		}

		++id;
	}

	alreadyCalled[&inst].push_back(inExecId);

	// call this on the nodes this calls
	for (const auto& conn : inst.outputExecConnections) {
		if (conn.first == nullptr) { continue; }
		res += validatePath(*conn.first, conn.second, alreadyCalled);
	}

	return res;
}
}  // anonymous namespace

Result validateFunctionNodeInputs(const GraphFunction& func) {
	Result res;

	// make sure they all get the context
	auto funcCtx =
	    res.addScopedContext({{"function", func.name()}, {"module", func.module().fullName()}});

	auto entry = func.entryNode();

	if (entry == nullptr) { return res; }

	std::unordered_map<const NodeInstance*, std::vector<int>> alreadyCalled;
	alreadyCalled.emplace(entry, std::vector<int>{});

	// no need to create a processed because you can't create a loop with entry in it

	for (const auto& conn : entry->outputExecConnections) {
		if (conn.first == nullptr) { continue; }
		res += validatePath(*conn.first, conn.second, alreadyCalled);
	}

	return res;
}

Result validateFunctionExecOutputs(const GraphFunction& func) {
	// make sure all exec outputs exist, and raise an error otherwise.
	// TODO (#70): quickfix to add return

	Result res;

	// make sure they all get the context
	auto funcCtx =
	    res.addScopedContext({{"function", func.name()}, {"module", func.module().fullName()}});

	for (const auto& nodepair : func.nodes()) {
		auto node = nodepair.second.get();

		auto id = 0ull;
		for (const auto& conn : node->outputExecConnections) {
			if (conn.second == ~0ull || conn.first == nullptr) {
				res.addEntry("EUKN", "Node is missing an output exec connection",
				             {{"Node ID", node->stringId()}, {"Missing ID", id}});
			}

			++id;
		}
	}

	return res;
}

Result validateFunctionEntryType(const GraphFunction& func) {
	Result res;

	auto entry = func.entryNode();
	if (!entry) {
		res.addEntry("EUKN", "Function  must have a valid entry node to validate the entry type",
		             {{"Function", func.name()}, {"Module", func.module().fullName()}});
		return res;
	}

	// make sure that the entry node has the right data types
	if (!std::equal(func.dataInputs().begin(), func.dataInputs().end(),
	                entry->type().dataOutputs().begin())) {
		nlohmann::json inFunc = nlohmann::json::array();
		for (auto& in : func.dataInputs()) {
			inFunc.push_back({{in.name, in.type.qualifiedName()}});
		}

		nlohmann::json inEntry = nlohmann::json::array();
		for (auto& in :
		     entry->type().dataOutputs()) {  // outputs to entry are inputs to the function
			inEntry.push_back({{in.name, in.type.qualifiedName()}});
		}

		res.addEntry("EUKN", "Inputs to function doesn't match function inputs",
		             {{"Function Inputs", inFunc}, {"Entry Inputs", inEntry}});
		return res;
	}

	return res;
}

Result validateFunctionExitTypes(const GraphFunction& func) {
	Result res;

	// make sure that each exit node has the right data types
	for (auto exitNode : func.nodesWithType("lang", "exit")) {
		if (!std::equal(func.dataOutputs().begin(), func.dataOutputs().end(),
		                exitNode->type().dataInputs().begin())) {
			nlohmann::json outFunc = nlohmann::json::array();
			for (auto& out : func.dataOutputs()) {
				outFunc.push_back({{out.name, out.type.qualifiedName()}});
			}

			nlohmann::json outExit = nlohmann::json::array();
			for (auto& out : exitNode->type().dataOutputs()) {
				// inputs to the exit are outputs to the function
				outExit.push_back({{out.name, out.type.qualifiedName()}});
			}

			res.addEntry("EUKN", "Outputs to function doesn't match function exit",
			             {{"Function Outputs", outFunc},
			              {"Exit Outputs", outExit},
			              {"Node ID", exitNode->stringId()}});
			return res;
		}
	}

	return res;
}

Result validateFunctionMainSignature(const GraphFunction& func) {
	Result res;

	if (func.execInputs().size() != 1) {
		res.addEntry("EUKN", "A main function must have exactly one exec in",
		             {{"Exec Inputs", func.execInputs()}});
	}
	if (func.execOutputs().size() != 1) {
		res.addEntry("EUKN", "A main function must have exactly one exec out",
		             {{"Exec Outputs", func.execOutputs()}});
	}

	if (!func.dataInputs().empty()) {
		auto dataJson = nlohmann::json::array();
		for (const auto& param : func.dataInputs()) {
			dataJson.push_back({{param.name, param.type.qualifiedName()}});
		}

		res.addEntry("EUKN", "A main function must have no data inputs",
		             {{"Data Inputs", dataJson}});
	}
	if (func.dataOutputs().size() != 1 ||
	    func.dataOutputs()[0].type.qualifiedName() != "lang:i32") {
		auto dataJson = nlohmann::json::array();
		for (const auto& param : func.dataOutputs()) {
			dataJson.push_back({{param.name, param.type.qualifiedName()}});
		}

		res.addEntry("EUKN", "A main function must have exactly one data output that's a lang:i32",
		             {{"Data Outputs", dataJson}});
	}
	return res;
}

}  // namespace chi
