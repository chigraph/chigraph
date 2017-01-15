#include "chig/NodeInstance.hpp"

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
	Expects(mType != nullptr);  // it's ok to initialize a NodeInstance without a function.

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
	  mContext{&other.context()} {
	mType->mNodeInstance = this;

	inputDataConnections.resize(type().dataInputs().size(), {nullptr, ~0});
	outputDataConnections.resize(type().dataOutputs().size(), {});

	inputExecConnections.resize(type().execInputs().size(), {});
	outputExecConnections.resize(type().execOutputs().size(), {nullptr, ~0});
}

NodeInstance& NodeInstance::operator=(const NodeInstance& other) {
	mType				 = other.type().clone();
	mType->mNodeInstance = this;
	mX					 = other.x();
	mY					 = other.y();
	mId					 = other.id() + "_";

	return *this;
}

void NodeInstance::setType(std::unique_ptr<NodeType> newType) {
	// delete exec connections that are out of range
	// start at one past the end
	for (size_t id = newType->execInputs().size(); id < inputExecConnections.size(); ++id) {
		while (inputExecConnections[id].size() != 0) {
			Expects(inputExecConnections[id][0].first);  // should never fail...
			disconnectExec(*inputExecConnections[id][0].first, inputExecConnections[id][0].second);
		}
	}
	inputExecConnections.resize(newType->execInputs().size());

	for (size_t id = newType->execOutputs().size(); id < outputExecConnections.size(); ++id) {
		auto& conn = outputExecConnections[id];
		if (conn.first) { disconnectExec(*this, id); }
	}
	outputExecConnections.resize(newType->execOutputs().size(), std::make_pair(nullptr, ~0));

	// trash all data connections TODO: don't actually trash them all keep good ones
	for (const auto& conn : inputDataConnections) {
		if (conn.first) { disconnectData(*conn.first, conn.second, *this); }
	}
	inputDataConnections.resize(newType->dataInputs().size(), std::make_pair(nullptr, ~0));

	size_t id = 0ull;
	for (const auto& connSlot : outputDataConnections) {
		while (connSlot.size() != 0) {
			Expects(connSlot[0].first);

			disconnectData(*this, id, *connSlot[0].first);
		}
		++id;
	}
	outputDataConnections.resize(newType->dataOutputs().size());

	mType				 = std::move(newType);
	mType->mNodeInstance = this;
}

Result connectData(NodeInstance& lhs, size_t connectionInputID, NodeInstance& rhs,
				   size_t connectionOutputID) {
	Result res = {};

	// make sure the connection exists
	// the input to the connection is the output to the node
	if (connectionInputID >= lhs.outputDataConnections.size()) {
		auto dataOutputs = nlohmann::json::array();
		for (auto& output : lhs.type().dataOutputs()) {
			dataOutputs.push_back({{output.second, output.first.qualifiedName()}});
		}

		res.addEntry("E22", "Output Data connection doesn't exist in node",
					 {{"Requested ID", connectionInputID},
					  {"Node Type", lhs.type().qualifiedName()},
					  {"Node JSON", rhs.type().toJSON()},
					  {"Node Output Data Connections", dataOutputs}});
	}
	if (connectionOutputID >= rhs.inputDataConnections.size()) {
		auto dataInputs = nlohmann::json::array();
		for (auto& output : rhs.type().dataInputs()) {
			dataInputs.push_back({{output.second, output.first.qualifiedName()}});
		}

		res.addEntry("E23", "Input Data connection doesn't exist in node",
					 {{"Requested ID", connectionOutputID},
					  {"Node Type", rhs.type().qualifiedName()},
					  {"Node JSON", rhs.type().toJSON()},
					  {"Node Input Data Connections", dataInputs}});
	}

	// if there are errors, back out
	if (!res) { return res; }
	// make sure the connection is of the right type
	if (lhs.type().dataOutputs()[connectionInputID].first !=
		rhs.type().dataInputs()[connectionOutputID].first) {
		res.addEntry(
			"E24", "Connecting data nodes with different types is invalid",
			{{"Left Hand Type", lhs.type().dataOutputs()[connectionInputID].first.qualifiedName()},
			 {"Right Hand Type", rhs.type().dataInputs()[connectionOutputID].first.qualifiedName()},
			 {"Left Node JSON", rhs.type().toJSON()},
			 {"Right Node JSON", rhs.type().toJSON()}});
		return res;
	}

	// if we are replacing a connection, disconnect it
	if (rhs.inputDataConnections[connectionOutputID].first != nullptr) {
		auto& extconn = rhs.inputDataConnections[connectionOutputID];

		// the node that we were connectd to's vector of output data connections
		auto& extconnvec = extconn.first->outputDataConnections[extconn.second];

		extconnvec.erase(std::find(extconnvec.begin(), extconnvec.end(),
								   std::make_pair(&rhs, connectionOutputID)));
	}

	lhs.outputDataConnections[connectionInputID].emplace_back(&rhs, connectionOutputID);
	rhs.inputDataConnections[connectionOutputID] = {&lhs, connectionInputID};

	return res;
}

Result connectExec(NodeInstance& lhs, size_t connectionInputID, NodeInstance& rhs,
				   size_t connectionOutputID) {
	Result res = {};

	// make sure the connection exists
	if (connectionInputID >= lhs.outputExecConnections.size()) {
		auto execOutputs = nlohmann::json::array();
		for (auto& output : lhs.type().execOutputs()) { execOutputs.push_back(output); }

		res.addEntry("E22", "Output exec connection doesn't exist in node",
					 {{"Requested ID", connectionInputID},
					  {"Node Type", lhs.type().qualifiedName()},
					  {"Node Output Exec Connections", execOutputs}});
	}
	if (connectionOutputID >= rhs.inputExecConnections.size()) {
		auto execInputs = nlohmann::json::array();
		for (auto& output : rhs.type().execInputs()) { execInputs.push_back(output); }

		res.addEntry("E23", "Input exec connection doesn't exist in node",
					 {{"Requested ID", connectionInputID},
					  {"Node Type", rhs.type().qualifiedName()},
					  {"Node Input Exec Connections", execInputs}

					 });
	}

	if (!res) { return res; }
	// if we are replacing a connection, disconnect it
	if (lhs.outputExecConnections[connectionInputID].first != nullptr) {
		auto& extconnvec =
			lhs.outputExecConnections[connectionInputID]
				.first->inputExecConnections[lhs.outputExecConnections[connectionInputID].second];

		extconnvec.erase(std::find(extconnvec.begin(), extconnvec.end(),
								   std::make_pair(&lhs, connectionOutputID)));
	}

	// connect it!
	lhs.outputExecConnections[connectionInputID] = {&rhs, connectionOutputID};
	rhs.inputExecConnections[connectionOutputID].emplace_back(&lhs, connectionOutputID);

	return res;
}

Result disconnectData(NodeInstance& lhs, size_t connectionInputID, NodeInstance& rhs) {
	Result res = {};

	if (connectionInputID >= lhs.outputDataConnections.size()) {
		auto dataOutputs = nlohmann::json::array();
		for (auto& output : lhs.type().dataOutputs()) {
			dataOutputs.push_back({{output.second, output.first.qualifiedName()}});
		}

		res.addEntry("E22", "Output data connection in node doesn't exist",
					 {{"Requested ID", connectionInputID},
					  {"Node Type", lhs.type().qualifiedName()},
					  {"Node JSON", rhs.type().toJSON()},
					  {"Node Output Data Connections", dataOutputs}});

		return res;
	}

	// find the connection
	auto iter = std::find_if(lhs.outputDataConnections[connectionInputID].begin(),
							 lhs.outputDataConnections[connectionInputID].end(),
							 [&](auto& pair) { return pair.first == &rhs; });

	if (iter == lhs.outputDataConnections[connectionInputID].end()) {
		res.addEntry("EUKN", "Cannot disconnect from connection that doesn't exist",
					 {{"Left node ID", lhs.id()},
					  {"Right node ID", rhs.id()},
					  {"Left dock ID", connectionInputID}});

		return res;
	}

	if (rhs.inputDataConnections.size() <= iter->second) {
		auto dataInputs = nlohmann::json::array();
		for (auto& output : rhs.type().dataInputs()) {
			dataInputs.push_back({{output.second, output.first.qualifiedName()}});
		}

		res.addEntry("E23", "Input Data connection doesn't exist in node",
					 {{"Requested ID", iter->second},
					  {"Node Type", rhs.type().qualifiedName()},
					  {"Node JSON", rhs.type().toJSON()},
					  {"Node Input Data Connections", dataInputs}});

		return res;
	}

	if (rhs.inputDataConnections[iter->second] != std::make_pair(&lhs, connectionInputID)) {
		res.addEntry("EUKN", "Cannot disconnect from connection that doesn't exist",
					 {{"Left node ID", lhs.id()}, {"Right node ID", rhs.id()}});

		return res;
	}

	// finally actually disconnect it
	rhs.inputDataConnections[iter->second] = {nullptr, ~0};
	lhs.outputDataConnections[connectionInputID].erase(iter);

	return res;
}

Result disconnectExec(NodeInstance& lhs, size_t connectionInputID) {
	Result res = {};

	if (connectionInputID >= lhs.outputExecConnections.size()) {
		auto execOutputs = nlohmann::json::array();
		for (auto& output : lhs.type().execOutputs()) { execOutputs.push_back(output); }

		res.addEntry("E22", "Output exec connection doesn't exist in node",
					 {{"Requested ID", connectionInputID},
					  {"Node Type", lhs.type().qualifiedName()},
					  {"Node Output Exec Connections", execOutputs}});
	}

	auto& lhsconn  = lhs.outputExecConnections[connectionInputID];
	auto& rhsconns = lhsconn.first->inputExecConnections[lhsconn.second];

	auto iter = std::find_if(rhsconns.begin(), rhsconns.end(), [&](auto pair) {
		return pair == std::make_pair(&lhs, connectionInputID);
	});

	if (iter == rhsconns.end()) {
		res.addEntry("EUKN", "Cannot disconnect an exec connection that doesn't connect back",
					 {{"Left node ID", lhs.id()}, {"Left node dock id", connectionInputID}});
		return res;
	}

	rhsconns.erase(iter);
	lhs.outputExecConnections[connectionInputID] = {nullptr, ~0};

	return res;
}

}  // namespace chig
