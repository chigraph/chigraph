/// \file FunctionValidator.cpp

#include "chig/FunctionValidator.hpp"
#include "chig/GraphFunction.hpp"
#include "chig/NodeInstance.hpp"

#include <unordered_map>

namespace chig {

Result validateFunction(const GraphFunction& func) {
	Result res;
	res += validateFunctionConnectionsAreTwoWay(func);
	res += validateFunctionNodeInputs(func);

	return res;
}

Result validateFunctionConnectionsAreTwoWay(const GraphFunction& func) {
	Result res;
	// make sure all connections connect back

	for (const auto& node : func.nodes()) {
		// go through input data
		auto id = 0ull;
		for (const auto& conn : node.second->inputDataConnections) {
			if (conn.first == nullptr) {
				res.addEntry("EUKN", "Node is missing an input data connection",
				             {{"nodeid", node.second->id()},
				              {"nodetype", node.second->type().qualifiedName()},
				              {"requested id", id}});
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
				             {{"Left Node", conn.first->id()},
				              {"Right Node", node.second->id()},
				              {"Right input ID", id}});
			}
			++id;
		}

		// output data
		id = 0ull;
		for (const auto& outputDataSlot : node.second->outputDataConnections) {
			// this connection type can make multiple connections, so two for loops are needed
			for (const auto& connection : outputDataSlot) {
				Expects(connection.first != nullptr);

				if (connection.first->inputDataConnections.size() <= connection.second) {
					res.addEntry(
					    "EUKN", "Input data port not found in node",
					    {{"nodeid", connection.first->id()}, {"requested id", connection.second}});
					continue;
				}

				auto& remoteConn = connection.first->inputDataConnections[connection.second];
				if (remoteConn.first != node.second.get() || remoteConn.second != id) {
					res.addEntry("EUKN", "Data connection doesn't connect back",
					             {{"Left Node", node.second->id()},
					              {"Right Node", connection.first->id()},
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
					res.addEntry(
					    "EUKN", "Exec connection doesn't connect back",
					    {{"Left Node", connection.first->id()},
					     {"Right Node", node.second->id()},
					     {"Left output ID", connection.second}});  // TODO: better diagnostics
				}
			}
			++id;
		}

		// output exec
		id = 0ull;
		for (const auto& connection : node.second->outputExecConnections) {
			bool connectsBack = false;

			if (connection.first == nullptr) { continue; }
			for (const auto& remoteConnection :
			     connection.first->inputExecConnections[connection.second]) {
				if (remoteConnection.second == id && remoteConnection.first == node.second.get()) {
					connectsBack = true;
					break;
				}
			}

			if (!connectsBack) {
				res.addEntry("EUKN", "Exec connection doesn't connect back",
				             {{"Left Node", node.second->id()},
				              {"Right Node", connection.first->id()},
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
	for (const auto& conn : inst.inputDataConnections) {
		if (conn.first == nullptr) {
			continue;
			// TODO: handle this
		}

		if (!conn.first->type().pure() && alreadyCalled.find(conn.first) == alreadyCalled.end()) {
			res.addEntry("EUKN", "Node that accepts data from another node is called first",
			             {{"nodeid", inst.id()}, {"othernodeid", conn.first->id()}});
		}
	}

	alreadyCalled[&inst].push_back(inExecId);

	// call this on the nodes this calls
	for (const auto& conn : inst.outputExecConnections) {
		if (conn.first == nullptr) { continue; }
		res += validatePath(*conn.first, conn.second, alreadyCalled);
	}

	return res;
}
} // anon namespace

Result validateFunctionNodeInputs(const GraphFunction& func) {
	Result res;

	auto entry = func.entryNode();

	if (entry == nullptr) {
		return res;
		// TODO: should this be an error?
	}

	std::unordered_map<const NodeInstance*, std::vector<int>> alreadyCalled;
	alreadyCalled.emplace(entry, std::vector<int>{});

	// no need to create a processed because you can't create a loop with entry in it

	for (const auto& conn : entry->outputExecConnections) {
		if (conn.first == nullptr) { continue; }
		res += validatePath(*conn.first, conn.second, alreadyCalled);
	}

	return res;
}
}
