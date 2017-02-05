/// \file GraphFunction.cpp

#include "chig/GraphFunction.hpp"
#include "chig/FunctionValidator.hpp"
#include "chig/GraphModule.hpp"
#include "chig/NameMangler.hpp"
#include "chig/NodeInstance.hpp"

#include <llvm/AsmParser/Parser.h>
#include <llvm/IR/DIBuilder.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/SourceMgr.h>

#include <boost/dynamic_bitset.hpp>
#include <boost/range/counting_range.hpp>
#include <boost/range/join.hpp>

namespace chig {
GraphFunction::GraphFunction(GraphModule& mod, gsl::cstring_span<> name,
                             std::vector<NamedDataType> dataIns,
                             std::vector<NamedDataType> dataOuts, std::vector<std::string> execIns,
                             std::vector<std::string> execOuts)
    : mModule{&mod},
      mContext{&mod.context()},
      mName{gsl::to_string(name)},
      mDataInputs(std::move(dataIns)),
      mDataOutputs(std::move(dataOuts)),
      mExecInputs(std::move(execIns)),
      mExecOutputs(std::move(execOuts)) {
	// TODO: check that it has at least 1 exec input and output
}

NodeInstance* GraphFunction::entryNode() const noexcept {
	auto matching = nodesWithType("lang", "entry");

	if (matching.size() == 1) {
		auto& vec = matching[0]->type().dataOutputs();
		// make sure it has the same signature as the method
		if (!std::equal(dataInputs().begin(), dataInputs().end(), vec.begin(), vec.end())) {
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

Result GraphFunction::insertNode(std::unique_ptr<NodeType> type, float x, float y,
                                 gsl::cstring_span<> id, NodeInstance** toFill) {
	Result res;

	// make sure the ID doesn't exist
	if (nodes().find(gsl::to_string(id)) != nodes().end()) {
		res.addEntry("EUKN", "Cannot have two nodes with the same ID",
		             {{"Requested ID", gsl::to_string(id)}});
		return res;
	}

	auto ptr = std::make_unique<NodeInstance>(this, std::move(type), x, y, id);

	auto emplaced = mNodes.emplace(gsl::to_string(id), std::move(ptr)).first;

	if (toFill != nullptr) { *toFill = emplaced->second.get(); }

	return res;
}

std::vector<NodeInstance*> GraphFunction::nodesWithType(gsl::cstring_span<> module,
                                                        gsl::cstring_span<> name) const noexcept {
	auto typeFinder = [&](auto& pair) {
		return pair.second->type().module().name() == module && pair.second->type().name() == name;
	};

	std::vector<NodeInstance*> ret;
	auto                       iter = std::find_if(mNodes.begin(), mNodes.end(), typeFinder);
	while (iter != mNodes.end()) {
		ret.emplace_back(iter->second.get());

		std::advance(iter, 1);  // don't process the same one twice!
		iter = std::find_if(iter, mNodes.end(), typeFinder);
	}

	return ret;
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
	nodes().erase(nodeToRemove->id());

	return res;
}

Result GraphFunction::createEntryNodeType(std::unique_ptr<NodeType>* toFill) const {
	Result res;

	nlohmann::json entry = nlohmann::json::object();

	auto& data = entry["data"];
	data       = nlohmann::json::array();
	for (auto in : dataInputs()) { data.push_back({{in.name, in.type.qualifiedName()}}); }

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
	for (auto out : dataOutputs()) { data.push_back({{out.name, out.type.qualifiedName()}}); }

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

void GraphFunction::addDataInput(const DataType& type, gsl::cstring_span<> name, int addAfter) {
	if (addAfter < mDataInputs.size()) {
		// +1 because emplace adds before
		mDataInputs.emplace(mDataInputs.cbegin() + addAfter + 1, gsl::to_string(name), type);
	} else {
		mDataInputs.emplace_back(gsl::to_string(name), type);
	}
	updateEntries();
}

void GraphFunction::removeDataInput(int idx) {
	if (idx < mDataInputs.size()) { mDataInputs.erase(mDataInputs.begin() + idx); }
	updateEntries();
}

void GraphFunction::modifyDataInput(int idx, const DataType& type,
                                    boost::optional<std::string> name) {
	if (idx < mDataInputs.size()) {
		if (type.valid()) { mDataInputs[idx].type = type; }
		if (name) { mDataInputs[idx].name = *name; }
	}
	updateEntries();
}

void GraphFunction::addDataOutput(const DataType& type, gsl::cstring_span<> name, int addAfter) {
	if (addAfter < mDataOutputs.size()) {
		// +1 because emplace adds before
		mDataOutputs.emplace(mDataOutputs.cbegin() + addAfter + 1, gsl::to_string(name), type);
	} else {
		mDataOutputs.emplace_back(gsl::to_string(name), type);
	}
	updateExits();
}

void GraphFunction::removeDataOutput(int idx) {
	if (idx < mDataOutputs.size()) { mDataOutputs.erase(mDataOutputs.begin() + idx); }
	updateExits();
}

void GraphFunction::modifyDataOutput(int idx, const DataType& type,
                                     boost::optional<std::string> name) {
	if (idx < mDataOutputs.size()) {
		if (type.valid()) { mDataOutputs[idx].type = type; }
		if (name) { mDataOutputs[idx].name = *name; }
	}
	updateExits();
}

llvm::FunctionType* GraphFunction::functionType() const {
	std::vector<llvm::Type*> arguments;
	arguments.reserve(1 + dataInputs().size() + dataOutputs().size());

	// this is for which exec input
	arguments.push_back(llvm::IntegerType::getInt32Ty(context().llvmContext()));

	for (const auto& p : dataInputs()) { arguments.push_back(p.type.llvmType()); }

	// make these pointers
	for (const auto& p : dataOutputs()) {
		arguments.push_back(llvm::PointerType::get(p.type.llvmType(), 0));
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
	if (idx < mExecInputs.size()) { mExecInputs.erase(mExecInputs.begin() + idx); }
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
	auto matching = nodesWithType("lang", "entry");

	for (auto entry : matching) {
		if (entry == nullptr) { return; }

		std::unique_ptr<NodeType> entryNodeType;
		createEntryNodeType(&entryNodeType);

		entry->setType(std::move(entryNodeType));
	}
}

void GraphFunction::updateExits() {
	for (const auto& exitNode : nodesWithType("lang", "exit")) {
		std::unique_ptr<NodeType> exitNodeType;
		createExitNodeType(&exitNodeType);

		exitNode->setType(std::move(exitNodeType));
	}
}

NamedDataType GraphFunction::localVariableFromName(gsl::cstring_span<> name) const {
	for (auto local : mLocalVariables) {
		if (local.name == name) { return local; }
	}

	return {};
}

NamedDataType GraphFunction::getOrCreateLocalVaraible(std::string name, DataType type,
                                                      bool* inserted) {
	auto local = localVariableFromName(name);

	if (local.valid()) {
		if (inserted != nullptr) { *inserted = false; }
		return local;
	}

	mLocalVariables.emplace_back(name, type);
	if (inserted != nullptr) { *inserted = true; }

	return mLocalVariables[mLocalVariables.size() - 1];
}

bool GraphFunction::removeLocalVaraible(gsl::cstring_span<> name) {
	auto iter = std::find_if(mLocalVariables.begin(), mLocalVariables.end(),
	                         [&](auto& toTest) { return toTest.name == name; });

	if (iter != mLocalVariables.end()) {
		mLocalVariables.erase(iter);
		return true;
	}
	return false;
}

}  // namespace chig
