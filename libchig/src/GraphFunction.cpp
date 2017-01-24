/// \file GraphFunction.cpp

#include "chig/GraphFunction.hpp"
#include "chig/FunctionValidator.hpp"
#include "chig/JsonModule.hpp"
#include "chig/NameMangler.hpp"

#include <llvm/AsmParser/Parser.h>
#include <llvm/IR/DIBuilder.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/SourceMgr.h>

#include <boost/dynamic_bitset.hpp>
#include <boost/range/counting_range.hpp>
#include <boost/range/join.hpp>

namespace chig {
GraphFunction::GraphFunction(JsonModule& mod, gsl::cstring_span<>          name,
                             std::vector<std::pair<DataType, std::string>> dataIns,
                             std::vector<std::pair<DataType, std::string>> dataOuts,
                             std::vector<std::string> execIns, std::vector<std::string> execOuts)
    : mModule{&mod},
      mContext{&mod.context()},
      mName{gsl::to_string(name)},
      mDataInputs(std::move(dataIns)),
      mDataOutputs(std::move(dataOuts)),
      mExecInputs(std::move(execIns)),
      mExecOutputs(std::move(execOuts)),
      mGraph{*this} {
	// TODO: check that it has at least 1 exec input and output
}

GraphFunction::~GraphFunction() = default;

GraphFunction::GraphFunction(JsonModule& module, const nlohmann::json& data, Result& res) : mGraph{*this} {
	if (!data.is_object()) {
		res.addEntry("E1", "Graph json isn't a JSON object", {});
		return;
	}
	// make sure it has a type element
	if (data.find("type") == data.end()) {
		res.addEntry("E2", R"(JSON in graph doesn't have a "type" element)", {});
		return;
	}
	if (data["type"] != "function") {
		res.addEntry("E3", "JSON in graph doesn't have a function type", {});
		return;
	}
	// make sure there is a name
	if (data.find("name") == data.end()) {
		res.addEntry("E4", "JSON in graph doesn't have a name parameter", {});
		return;
	}
	std::string name = data["name"];

	if (data.find("data_inputs") == data.end() || !data["data_inputs"].is_array()) {
		res.addEntry("E43", "JSON in graph doesn't have an data_inputs array", {});
		return;
	}

	std::vector<std::pair<DataType, std::string>> datainputs;
	for (auto param : data["data_inputs"]) {
		for (auto iter = param.begin(); iter != param.end(); ++iter) {
			std::string qualifiedType = iter.value();
			std::string docString     = iter.key();

			std::string moduleName, name;
			std::tie(moduleName, name) = parseColonPair(qualifiedType);

			DataType ty;
			res += module.context().typeFromModule(moduleName, name, &ty);

			if (!res) { return; }

			datainputs.emplace_back(ty, docString);
		}
	}

	if (data.find("data_outputs") == data.end() || !data["data_outputs"].is_array()) {
		res.addEntry("E44", "JSON in graph doesn't have an data_outputs array", {});
		return;
	}

	std::vector<std::pair<DataType, std::string>> dataoutputs;
	for (auto param : data["data_outputs"]) {
		for (auto iter = param.begin(); iter != param.end(); ++iter) {
			std::string qualifiedType = iter.value();
			std::string docString     = iter.key();

			std::string moduleName, name;
			std::tie(moduleName, name) = parseColonPair(qualifiedType);

			DataType ty;
			res += module.context().typeFromModule(moduleName, name, &ty);

			if (!res) { return; }

			dataoutputs.emplace_back(ty, docString);
		}
	}

	// get exec I/O
	if (data.find("exec_inputs") == data.end() || !data["exec_inputs"].is_array()) {
		res.addEntry("EUKN", "JSON in graph doesn't have an exec_inputs array", {});
		return;
	}

	std::vector<std::string> execinputs;
	for (const auto& param : data["exec_inputs"]) {
		std::string name = param;

		execinputs.emplace_back(name);
	}

	if (data.find("exec_outputs") == data.end() || !data["exec_outputs"].is_array()) {
		res.addEntry("EUKN", "JSON in graph doesn't have an data_outputs array", {});
		return;
	}

	std::vector<std::string> execoutputs;
	for (const auto& param : data["exec_outputs"]) {
		std::string name = param;

		execoutputs.emplace_back(name);
	}

	// construct it
	mModule      = &module;
	mContext     = &module.context();
	mName        = name;
	mDataInputs  = std::move(datainputs);
	mDataOutputs = std::move(dataoutputs);
	mExecInputs  = std::move(execinputs);
	mExecOutputs = std::move(execoutputs);

	mSource = data;
}

Result GraphFunction::toJSON(nlohmann::json* toFill) const {
	Result res;

	*toFill        = nlohmann::json{};
	auto& jsonData = *toFill;

	jsonData["type"] = "function";
	jsonData["name"] = name();

	auto& datainputsjson = jsonData["data_inputs"];
	datainputsjson       = nlohmann::json::array();

	for (auto& in : dataInputs()) {
		datainputsjson.push_back({{in.second, in.first.qualifiedName()}});
	}

	auto& dataoutputsjson = jsonData["data_outputs"];
	dataoutputsjson       = nlohmann::json::array();

	for (auto& out : dataOutputs()) {
		dataoutputsjson.push_back({{out.second, out.first.qualifiedName()}});
	}

	auto& execinputsjson = jsonData["exec_inputs"];
	execinputsjson       = nlohmann::json::array();

	for (auto& in : execInputs()) { execinputsjson.push_back(in); }

	auto& execoutputsjson = jsonData["exec_outputs"];
	execoutputsjson       = nlohmann::json::array();

	for (auto& out : execOutputs()) { execoutputsjson.push_back(out); }

	res += graph().toJson(toFill);

	return res;
}

NodeInstance* GraphFunction::entryNode() const noexcept {
	auto matching = graph().nodesWithType("lang", "entry");

	if (matching.size() == 1) {
		auto span = matching[0]->type().dataOutputs();
		// make sure it has the same signature as the method
		if (!std::equal(dataInputs().begin(), dataInputs().end(), span.begin(), span.end())) {
			return nullptr;
		}
		// make sure it has the same exec names and size
		if (!std::equal(execInputs().begin(), execInputs().end(),
		                matching[0]->type().execOutputs().begin())) {
			return nullptr;
		}
		return matching[0];
	}
	return nullptr;
}

Result GraphFunction::insertNode(gsl::cstring_span<> moduleName, gsl::cstring_span<> typeName,
                                 const nlohmann::json& typeJSON, float x, float y,
                                 gsl::cstring_span<> id, NodeInstance** toFill) {
	std::unique_ptr<NodeType> nodeType;
	Result res = context().nodeTypeFromModule(moduleName, typeName, typeJSON, &nodeType);

	if (!res) { return res; }
	res += insertNode(std::move(nodeType), x, y, id, toFill);
	return res;
}

Result GraphFunction::removeNode(NodeInstance* nodeToRemove) {
	Result res;

	// disconnect it's connections

	// disconnect input exec
	for (const auto& execSlot : nodeToRemove->inputExecConnections) {
		for (const auto& pair : execSlot) {
			if (pair.first != nullptr) { res += disconnectExec(*pair.first, pair.second); }
		}
	}
	// disconnect output exec
	auto ID = 0ull;
	for (const auto& pair : nodeToRemove->outputExecConnections) {
		if (pair.first != nullptr) { res += disconnectExec(*nodeToRemove, ID); }
		++ID;
	}

	// disconnect input data
	for (const auto& pair : nodeToRemove->inputDataConnections) {
		if (pair.first != nullptr) {
			res += disconnectData(*pair.first, pair.second, *nodeToRemove);
		}
	}

	// disconnect output data
	ID = 0ull;
	for (const auto& dataSlot : nodeToRemove->outputDataConnections) {
		for (const auto& pair : dataSlot) {
			if (pair.first != nullptr) { disconnectData(*nodeToRemove, ID, *pair.first); }
		}
		++ID;
	}
	// then delete the node
	graph().nodes().erase(nodeToRemove->id());

	return res;
}

Result GraphFunction::createEntryNodeType(std::unique_ptr<NodeType>* toFill) const {
	Result res;

	nlohmann::json entry = nlohmann::json::object();

	auto& data = entry["data"];
	data       = nlohmann::json::array();
	for (auto in : dataInputs()) { data.push_back({{in.second, in.first.qualifiedName()}}); }

	auto& exec = entry["exec"];
	exec       = nlohmann::json::array();
	for (const auto& in : execInputs()) { exec.push_back(in); }

	res += context().nodeTypeFromModule("lang", "entry", entry, toFill);

	return res;
}

Result GraphFunction::createExitNodeType(std::unique_ptr<NodeType>* toFill) const {
	Result res;

	nlohmann::json exit = nlohmann::json::object();

	auto& data = exit["data"];
	data       = nlohmann::json::array();
	for (auto out : dataOutputs()) { data.push_back({{out.second, out.first.qualifiedName()}}); }

	auto& exec = exit["exec"];
	exec       = nlohmann::json::array();
	for (const auto& out : execOutputs()) { exec.push_back(out); }

	res += context().nodeTypeFromModule("lang", "exit", exit, toFill);

	return res;
}

Result GraphFunction::getOrInsertEntryNode(float x, float y, gsl::cstring_span<> id,
                                           NodeInstance** toFill) {
	Result res;

	if (auto ent = entryNode()) {
		if (toFill != nullptr) { *toFill = ent; }
		return {};
	}

	std::unique_ptr<NodeType> entryNodeType;
	res += createEntryNodeType(&entryNodeType);

	res += insertNode(std::move(entryNodeType), x, y, id, toFill);

	return res;
}

Result GraphFunction::loadGraph() {
	Result res;
	mGraph = Graph(*this, mSource, res);

	return res;
}

void GraphFunction::addDataInput(const DataType& type, gsl::cstring_span<> name, int addAfter) {
	if (addAfter < mDataInputs.size()) {
		// +1 because emplace adds before
		mDataInputs.emplace(mDataInputs.cbegin() + addAfter + 1, type, gsl::to_string(name));
	} else {
		mDataInputs.emplace_back(type, gsl::to_string(name));
	}
	updateEntries();
}

void GraphFunction::removeDataInput(int idx) {
	if (idx < mDataInputs.size()) { mDataInputs.erase(mDataInputs.begin() + idx); }
	updateEntries();
}

void GraphFunction::modifyDataInput(int idx, const DataType& type,
                                    boost::optional<gsl::cstring_span<>> name) {
	if (idx < mDataInputs.size()) {
		if (type.valid()) { mDataInputs[idx].first = type; }
		if (name) { mDataInputs[idx].second = gsl::to_string(*name); }
	}
	updateEntries();
}

void GraphFunction::addDataOutput(const DataType& type, gsl::cstring_span<> name, int addAfter) {
	if (addAfter < mDataOutputs.size()) {
		// +1 because emplace adds before
		mDataOutputs.emplace(mDataOutputs.cbegin() + addAfter + 1, type, gsl::to_string(name));
	} else {
		mDataOutputs.emplace_back(type, gsl::to_string(name));
	}
	updateExits();
}

void GraphFunction::removeDataOutput(int idx) {
	if (idx < mDataOutputs.size()) { mDataOutputs.erase(mDataOutputs.begin() + idx); }
	updateExits();
}

void GraphFunction::modifyDataOutput(int idx, const DataType& type,
                                     boost::optional<gsl::cstring_span<>> name) {
	if (idx < mDataOutputs.size()) {
		if (type.valid()) { mDataOutputs[idx].first = type; }
		if (name) { mDataOutputs[idx].second = gsl::to_string(*name); }
	}
	updateExits();
}

llvm::FunctionType* GraphFunction::functionType() const {
	std::vector<llvm::Type*> arguments;
	arguments.reserve(1 + dataInputs().size() + dataOutputs().size());

	// this is for which exec input
	arguments.push_back(llvm::IntegerType::getInt32Ty(context().llvmContext()));

	for (const auto& p : dataInputs()) { arguments.push_back(p.first.llvmType()); }

	// make these pointers
	for (const auto& p : dataOutputs()) {
		arguments.push_back(llvm::PointerType::get(p.first.llvmType(), 0));
	}

	return llvm::FunctionType::get(llvm::IntegerType::getInt32Ty(context().llvmContext()),
	                               arguments, false);
}

void GraphFunction::addExecInput(gsl::cstring_span<> name, int addAfter) {
	if (addAfter < mExecInputs.size()) {
		// +1 because emplace adds before
		mExecInputs.emplace(mExecInputs.cbegin() + addAfter + 1, gsl::to_string(name));
	} else {
		mExecInputs.emplace_back(gsl::to_string(name));
	}
	updateEntries();
}

void GraphFunction::removeExecInput(int idx) {
	if (idx < mExecOutputs.size()) { mExecOutputs.erase(mExecOutputs.begin() + idx); }
	updateEntries();
}

void GraphFunction::modifyExecInput(int idx, gsl::cstring_span<> name) {
	if (idx < mExecInputs.size()) { mExecInputs[idx] = gsl::to_string(name); }
	updateEntries();
}

void GraphFunction::addExecOutput(gsl::cstring_span<> name, int addAfter) {
	if (addAfter < mExecOutputs.size()) {
		// +1 because emplace adds before
		mExecOutputs.emplace(mExecOutputs.cbegin() + addAfter + 1, gsl::to_string(name));
	} else {
		mExecOutputs.emplace_back(gsl::to_string(name));
	}
	updateExits();
}

void GraphFunction::removeExecOutput(int idx) {
	if (idx < mExecOutputs.size()) { mExecOutputs.erase(mExecOutputs.begin() + idx); }
	updateExits();
}

void GraphFunction::modifyExecOutput(int idx, gsl::cstring_span<> name) {
	if (idx < mExecOutputs.size()) { mExecOutputs[idx] = gsl::to_string(name); }
	updateExits();
}

void GraphFunction::updateEntries() {
	auto matching = graph().nodesWithType("lang", "entry");

	for (auto entry : matching) {
		if (entry == nullptr) { return; }

		std::unique_ptr<NodeType> entryNodeType;
		createEntryNodeType(&entryNodeType);

		entry->setType(std::move(entryNodeType));
	}
}

void GraphFunction::updateExits() {
	for (const auto& exitNode : graph().nodesWithType("lang", "exit")) {
		std::unique_ptr<NodeType> exitNodeType;
		createExitNodeType(&exitNodeType);

		exitNode->setType(std::move(exitNodeType));
	}
}

}  // namespace chig
