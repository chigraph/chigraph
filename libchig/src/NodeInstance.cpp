/// \file NodeInstance.cpp

#include "chig/NodeInstance.hpp"
#include "chig/FunctionValidator.hpp"

#include <gsl/gsl>

namespace chig {
NodeInstance::NodeInstance(GraphFunction* func, std::unique_ptr<NodeType> nodeType, float posX,
                           float posY, gsl::cstring_span<> nodeID)
    : mType{std::move(nodeType)},
      mX{posX},
      mY{posY},
      mId{gsl::to_string(nodeID)},
      mContext{&mType->context()},
      mFunction{func} {
	Expects(mType != nullptr && mFunction != nullptr);

	mType->mNodeInstance = this;

	inputDataConnections.resize(type().dataInputs().size(), {nullptr, ~0});
	outputDataConnections.resize(type().dataOutputs().size(), {});

	inputExecConnections.resize(type().execInputs().size(), {});
	outputExecConnections.resize(type().execOutputs().size(), {nullptr, ~0});
}

NodeInstance::NodeInstance(const NodeInstance& other, std::string id)
    : mType(other.type().clone()),
      mX{other.x()},
      mY{other.y()},
      mId{std::move(id)},
      mContext{&other.context()},
      mFunction{&other.function()} {
	Expects(mType != nullptr && mFunction != nullptr);

	mType->mNodeInstance = this;

	inputDataConnections.resize(type().dataInputs().size(), {nullptr, ~0});
	outputDataConnections.resize(type().dataOutputs().size(), {});

	inputExecConnections.resize(type().execInputs().size(), {});
	outputExecConnections.resize(type().execOutputs().size(), {nullptr, ~0});
}

NodeInstance& NodeInstance::operator=(const NodeInstance& other) {
	mType                = other.type().clone();
	mType->mNodeInstance = this;
	mX                   = other.x();
	mY                   = other.y();
	mId                  = other.id() + "_";

	return *this;
}

void NodeInstance::setType(std::unique_ptr<NodeType> newType) {
	// delete exec connections that are out of range
	// start at one past the end
	for (size_t id = newType->execInputs().size(); id < inputExecConnections.size(); ++id) {
		while (!inputExecConnections[id].empty()) {
			Expects(inputExecConnections[id][0].first);  // should never fail...
			disconnectExec(*inputExecConnections[id][0].first, inputExecConnections[id][0].second);
		}
	}
	inputExecConnections.resize(newType->execInputs().size());

	for (size_t id = newType->execOutputs().size(); id < outputExecConnections.size(); ++id) {
		auto& conn = outputExecConnections[id];
		if (conn.first != nullptr) { disconnectExec(*this, id); }
	}
	outputExecConnections.resize(newType->execOutputs().size(), std::make_pair(nullptr, ~0));

	auto id = 0ull;
	for (const auto& conn : inputDataConnections) {
		if (conn.first != nullptr && newType->dataInputs().size() > id &&
		      !(type().dataInputs()[id].type == newType->dataInputs()[id].type)) {
			disconnectData(*conn.first, conn.second, *this);
		}
		++id;
	}
	inputDataConnections.resize(newType->dataInputs().size(), std::make_pair(nullptr, ~0));

	id = 0ull;
	for (const auto& connSlot : outputDataConnections) {
		// keep the connections if they're still good
		if (newType->dataOutputs().size() > id &&
		    type().dataOutputs()[id].type == newType->dataOutputs()[id].type) {
			continue;
		}

		while (!connSlot.empty()) {
			Expects(connSlot[0].first);

			disconnectData(*this, id, *connSlot[0].first);
		}
		++id;
	}
	outputDataConnections.resize(newType->dataOutputs().size());

	mType                = std::move(newType);
	mType->mNodeInstance = this;
}

Result connectData(NodeInstance& lhs, size_t lhsConnID, NodeInstance& rhs, size_t rhsConnID) {
	Result res = {};

	if (&lhs.function() != &rhs.function()) {
		res.addEntry("EUKN", "Cannot connect two nodes of different graphs",
		             {});  // TODO: better errors
		return res;
	}

	// make sure the connection exists
	// the input to the connection is the output to the node
	if (lhsConnID >= lhs.outputDataConnections.size()) {
		auto dataOutputs = nlohmann::json::array();
		for (auto& output : lhs.type().dataOutputs()) {
			dataOutputs.push_back({{output.name, output.type.qualifiedName()}});
		}

		res.addEntry("E22", "Output Data connection doesn't exist in node",
		             {{"Requested ID", lhsConnID},
		              {"Node Type", lhs.type().qualifiedName()},
		              {"Node JSON", rhs.type().toJSON()},
		              {"Node Output Data Connections", dataOutputs}});
	}
	if (rhsConnID >= rhs.inputDataConnections.size()) {
		auto dataInputs = nlohmann::json::array();
		for (auto& output : rhs.type().dataInputs()) {
			dataInputs.push_back({{output.name, output.type.qualifiedName()}});
		}

		res.addEntry("E23", "Input Data connection doesn't exist in node",
		             {{"Requested ID", rhsConnID},
		              {"Node Type", rhs.type().qualifiedName()},
		              {"Node JSON", rhs.type().toJSON()},
		              {"Node Input Data Connections", dataInputs}});
	}

	// if there are errors, back out
	if (!res) { return res; }
	// make sure the connection is of the right type
	if (lhs.type().dataOutputs()[lhsConnID].type != rhs.type().dataInputs()[rhsConnID].type) {
		res.addEntry("E24", "Connecting data nodes with different types is invalid",
		             {{"Left Hand Type", lhs.type().dataOutputs()[lhsConnID].type.qualifiedName()},
		              {"Right Hand Type", rhs.type().dataInputs()[rhsConnID].type.qualifiedName()},
		              {"Left Node JSON", rhs.type().toJSON()},
		              {"Right Node JSON", rhs.type().toJSON()}});
		return res;
	}

	// if we are replacing a connection, disconnect it
	if (rhs.inputDataConnections[rhsConnID].first != nullptr) {
		res += disconnectData(lhs, lhsConnID, rhs);
		if (!res) { return res; }
	}

	lhs.outputDataConnections[lhsConnID].emplace_back(&rhs, rhsConnID);
	rhs.inputDataConnections[rhsConnID] = {&lhs, lhsConnID};

	return res;
}

Result connectExec(NodeInstance& lhs, size_t lhsConnID, NodeInstance& rhs, size_t rhsConnID) {
	Result res = {};

	if (&lhs.function() != &rhs.function()) {
		res.addEntry("EUKN", "Cannot connect two nodes of different graphs",
		             {});  // TODO: better errors
		return res;
	}

	// make sure the connection exists
	if (lhsConnID >= lhs.outputExecConnections.size()) {
		auto execOutputs = nlohmann::json::array();
		for (auto& output : lhs.type().execOutputs()) { execOutputs.push_back(output); }

		res.addEntry("E22", "Output exec connection doesn't exist in node",
		             {{"Requested ID", lhsConnID},
		              {"Node Type", lhs.type().qualifiedName()},
		              {"Node Output Exec Connections", execOutputs}});
	}
	if (rhsConnID >= rhs.inputExecConnections.size()) {
		auto execInputs = nlohmann::json::array();
		for (auto& output : rhs.type().execInputs()) { execInputs.push_back(output); }

		res.addEntry("E23", "Input exec connection doesn't exist in node",
		             {{"Requested ID", lhsConnID},
		              {"Node Type", rhs.type().qualifiedName()},
		              {"Node Input Exec Connections", execInputs}

		             });
	}

	if (!res) { return res; }
	// if we are replacing a connection, disconnect it
	if (lhs.outputExecConnections[lhsConnID].first != nullptr) {
		res += disconnectExec(lhs, lhsConnID);
		if (!res) { return res; }
	}

	// connect it!
	lhs.outputExecConnections[lhsConnID] = {&rhs, rhsConnID};
	rhs.inputExecConnections[rhsConnID].emplace_back(&lhs, lhsConnID);

	return res;
}

Result disconnectData(NodeInstance& lhs, size_t lhsConnID, NodeInstance& rhs) {
	Result res = {};

	if (&lhs.function() != &rhs.function()) {
		res.addEntry("EUKN", "Cannot disconect two nodes of different graphs",
		             {});  // TODO: better errors
		return res;
	}

	if (lhsConnID >= lhs.outputDataConnections.size()) {
		auto dataOutputs = nlohmann::json::array();
		for (auto& output : lhs.type().dataOutputs()) {
			dataOutputs.push_back({{output.name, output.type.qualifiedName()}});
		}

		res.addEntry("E22", "Output data connection in node doesn't exist",
		             {{"Requested ID", lhsConnID},
		              {"Node Type", lhs.type().qualifiedName()},
		              {"Node JSON", rhs.type().toJSON()},
		              {"Node Output Data Connections", dataOutputs}});

		return res;
	}

	// find the connection
	auto iter = std::find_if(lhs.outputDataConnections[lhsConnID].begin(),
	                         lhs.outputDataConnections[lhsConnID].end(),
	                         [&](auto& pair) { return pair.first == &rhs; });

	if (iter == lhs.outputDataConnections[lhsConnID].end()) {
		res.addEntry(
		    "EUKN", "Cannot disconnect from connection that doesn't exist",
		    {{"Left node ID", lhs.id()}, {"Right node ID", rhs.id()}, {"Left dock ID", lhsConnID}});

		return res;
	}

	if (rhs.inputDataConnections.size() <= iter->second) {
		auto dataInputs = nlohmann::json::array();
		for (auto& output : rhs.type().dataInputs()) {
			dataInputs.push_back({{output.name, output.type.qualifiedName()}});
		}

		res.addEntry("E23", "Input Data connection doesn't exist in node",
		             {{"Requested ID", iter->second},
		              {"Node Type", rhs.type().qualifiedName()},
		              {"Node JSON", rhs.type().toJSON()},
		              {"Node Input Data Connections", dataInputs}});

		return res;
	}

	if (rhs.inputDataConnections[iter->second] != std::make_pair(&lhs, lhsConnID)) {
		res.addEntry("EUKN", "Cannot disconnect from connection that doesn't exist",
		             {{"Left node ID", lhs.id()}, {"Right node ID", rhs.id()}});

		return res;
	}

	// finally actually disconnect it
	rhs.inputDataConnections[iter->second] = {nullptr, ~0};
	lhs.outputDataConnections[lhsConnID].erase(iter);

	return res;
}

Result disconnectExec(NodeInstance& lhs, size_t lhsConnID) {
	Result res = {};

	if (lhsConnID >= lhs.outputExecConnections.size()) {
		auto execOutputs = nlohmann::json::array();
		for (auto& output : lhs.type().execOutputs()) { execOutputs.push_back(output); }

		res.addEntry("E22", "Output exec connection doesn't exist in node",
		             {{"Requested ID", lhsConnID},
		              {"Node Type", lhs.type().qualifiedName()},
		              {"Node Output Exec Connections", execOutputs}});
	}

	auto& lhsconn  = lhs.outputExecConnections[lhsConnID];
	auto& rhsconns = lhsconn.first->inputExecConnections[lhsconn.second];

	auto iter = std::find_if(rhsconns.begin(), rhsconns.end(),
	                         [&](auto pair) { return pair == std::make_pair(&lhs, lhsConnID); });

	if (iter == rhsconns.end()) {
		res.addEntry("EUKN", "Cannot disconnect an exec connection that doesn't connect back",
		             {{"Left node ID", lhs.id()}, {"Left node dock id", lhsConnID}});
		return res;
	}

	rhsconns.erase(iter);
	lhs.outputExecConnections[lhsConnID] = {nullptr, ~0};

	return res;
}

}  // namespace chig
