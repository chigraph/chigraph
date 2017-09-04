/// \file GraphFunction.cpp

#include "chi/GraphFunction.hpp"
#include "chi/Context.hpp"
#include "chi/DataType.hpp"
#include "chi/FunctionValidator.hpp"
#include "chi/GraphModule.hpp"
#include "chi/NameMangler.hpp"
#include "chi/NodeInstance.hpp"
#include "chi/NodeType.hpp"
#include "chi/Support/Result.hpp"

#include <llvm/AsmParser/Parser.h>
#include <llvm/IR/DIBuilder.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/SourceMgr.h>

#include <boost/dynamic_bitset.hpp>
#include <boost/range/counting_range.hpp>
#include <boost/range/join.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace chi {
GraphFunction::GraphFunction(GraphModule& mod, std::string name, std::vector<NamedDataType> dataIns,
                             std::vector<NamedDataType> dataOuts, std::vector<std::string> execIns,
                             std::vector<std::string> execOuts)
    : mModule{&mod},
      mContext{&mod.context()},
      mName{std::move(name)},
      mDataInputs(std::move(dataIns)),
      mDataOutputs(std::move(dataOuts)),
      mExecInputs(std::move(execIns)),
      mExecOutputs(std::move(execOuts)) {
	// TODO(#66): check that it has at least 1 exec input and output
}

NodeInstance* GraphFunction::nodeByID(const boost::uuids::uuid& id) const {
	auto iter = nodes().find(id);
	if (iter != nodes().end()) { return iter->second.get(); }
	return nullptr;
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
		auto foundExecOutputs = matching[0]->type().execOutputs();
		if (!std::equal(execInputs().begin(), execInputs().end(), foundExecOutputs.begin(),
		                foundExecOutputs.end())) {
			return nullptr;
		}
		return matching[0];
	}
	return nullptr;
}

Result GraphFunction::insertNode(std::unique_ptr<NodeType> type, float x, float y,
                                 boost::uuids::uuid id, NodeInstance** toFill) {
	// invalidate the cache
	module().updateLastEditTime();

	Result res;

	// make sure the ID doesn't exist
	if (nodes().find(id) != nodes().end()) {
		res.addEntry("E47", "Cannot have two nodes with the same ID",
		             {{"Requested ID", boost::uuids::to_string(id)}});
		return res;
	}

	auto ptr = std::make_unique<NodeInstance>(this, std::move(type), x, y, id);

	auto emplaced = mNodes.emplace(id, std::move(ptr)).first;

	if (toFill != nullptr) { *toFill = emplaced->second.get(); }

	return res;
}

std::vector<NodeInstance*> GraphFunction::nodesWithType(const boost::filesystem::path& module,
                                                        boost::string_view name) const noexcept {
	std::vector<NodeInstance*> ret;
	for (const auto& node : mNodes) {
		if (node.second->type().module().fullName() == module &&
		    node.second->type().name() == name) {
			ret.push_back(node.second.get());
		}
	}

	return ret;
}

Result GraphFunction::insertNode(const boost::filesystem::path& moduleName,
                                 boost::string_view typeName, const nlohmann::json& typeJSON,
                                 float x, float y, boost::uuids::uuid id, NodeInstance** toFill) {
	// invalidate the cache
	module().updateLastEditTime();

	std::unique_ptr<NodeType> nodeType;
	Result res = context().nodeTypeFromModule(moduleName, typeName, typeJSON, &nodeType);

	if (!res) { return res; }
	res += insertNode(std::move(nodeType), x, y, id, toFill);
	return res;
}

Result GraphFunction::removeNode(NodeInstance& nodeToRemove) {
	// invalidate the cache
	module().updateLastEditTime();

	Result res;

	// disconnect it's connections

	// disconnect input exec
	for (const auto& execSlot : nodeToRemove.inputExecConnections) {
		for (const auto& pair : execSlot) {
			if (pair.first != nullptr) { res += disconnectExec(*pair.first, pair.second); }
		}
	}
	// disconnect output exec
	auto ID = 0ull;
	for (const auto& pair : nodeToRemove.outputExecConnections) {
		if (pair.first != nullptr) { res += disconnectExec(nodeToRemove, ID); }
		++ID;
	}

	// disconnect input data
	for (const auto& pair : nodeToRemove.inputDataConnections) {
		if (pair.first != nullptr) {
			res += disconnectData(*pair.first, pair.second, nodeToRemove);
		}
	}

	// disconnect output data
	ID = 0ull;
	for (const auto& dataSlot : nodeToRemove.outputDataConnections) {
		for (const auto& pair : dataSlot) {
			if (pair.first != nullptr) { disconnectData(nodeToRemove, ID, *pair.first); }
		}
		++ID;
	}
	// then delete the node
	nodes().erase(nodeToRemove.id());

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

Result GraphFunction::getOrInsertEntryNode(float x, float y, boost::uuids::uuid id,
                                           NodeInstance** toFill) {
	Result res;

	if (auto ent = entryNode()) {
		if (toFill != nullptr) { *toFill = ent; }
		return res;
	}

	// invalidate the cache
	module().updateLastEditTime();

	std::unique_ptr<NodeType> entryNodeType;
	res += createEntryNodeType(&entryNodeType);

	res += insertNode(std::move(entryNodeType), x, y, id, toFill);

	return res;
}

void GraphFunction::addDataInput(const DataType& type, std::string name, size_t addBefore) {
	// invalidate the cache
	module().updateLastEditTime();

	if (addBefore < mDataInputs.size()) {
		mDataInputs.emplace(mDataInputs.cbegin() + addBefore, std::move(name), type);
	} else {
		mDataInputs.emplace_back(std::move(name), type);
	}
	updateEntries();
}

void GraphFunction::removeDataInput(size_t idx) {
	// invalidate the cache
	module().updateLastEditTime();

	if (idx < mDataInputs.size()) { mDataInputs.erase(mDataInputs.begin() + idx); }
	updateEntries();
}

void GraphFunction::renameDataInput(size_t idx, std::string newName) {
	// invalidate the cache
	module().updateLastEditTime();

	if (idx < mDataInputs.size()) { mDataInputs[idx].name = std::move(newName); }
	updateEntries();
}

void GraphFunction::retypeDataInput(size_t idx, DataType newType) {
	// invalidate the cache
	module().updateLastEditTime();

	if (idx < mDataInputs.size()) { mDataInputs[idx].type = std::move(newType); }
	updateEntries();
}

void GraphFunction::addDataOutput(const DataType& type, std::string name, size_t addBefore) {
	// invalidate the cache
	module().updateLastEditTime();

	if (addBefore < mDataOutputs.size()) {
		mDataOutputs.emplace(mDataOutputs.cbegin() + addBefore, std::move(name), type);
	} else {
		mDataOutputs.emplace_back(std::move(name), type);
	}
	updateExits();
}

void GraphFunction::removeDataOutput(size_t idx) {
	// invalidate the cache
	module().updateLastEditTime();

	if (idx < mDataOutputs.size()) { mDataOutputs.erase(mDataOutputs.begin() + idx); }
	updateExits();
}

void GraphFunction::renameDataOutput(size_t idx, std::string newName) {
	// invalidate the cache
	module().updateLastEditTime();

	if (idx < mDataOutputs.size()) { mDataOutputs[idx].name = std::move(newName); }
	updateExits();
}

void GraphFunction::retypeDataOutput(size_t idx, DataType newType) {
	if (idx < mDataOutputs.size()) { mDataOutputs[idx].type = std::move(newType); }
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

void GraphFunction::addExecInput(std::string name, size_t addBefore) {
	// invalidate the cache
	module().updateLastEditTime();

	if (addBefore < mExecInputs.size()) {
		// +1 because emplace adds before
		mExecInputs.emplace(mExecInputs.cbegin() + addBefore, std::move(name));
	} else {
		mExecInputs.emplace_back(std::move(name));
	}
	updateEntries();
}

void GraphFunction::removeExecInput(size_t idx) {
	// invalidate the cache
	module().updateLastEditTime();

	if (idx < mExecInputs.size()) { mExecInputs.erase(mExecInputs.begin() + idx); }
	updateEntries();
}

void GraphFunction::renameExecInput(size_t idx, std::string name) {
	// invalidate the cache
	module().updateLastEditTime();

	if (idx < mExecInputs.size()) { mExecInputs[idx] = std::move(name); }
	updateEntries();
}

void GraphFunction::addExecOutput(std::string name, size_t addBefore) {
	// invalidate the cache
	module().updateLastEditTime();

	if (addBefore < mExecOutputs.size()) {
		// +1 because emplace adds before
		mExecOutputs.emplace(mExecOutputs.cbegin() + addBefore, std::move(name));
	} else {
		mExecOutputs.emplace_back(std::move(name));
	}
	updateExits();
}

void GraphFunction::removeExecOutput(size_t idx) {
	// invalidate the cache
	module().updateLastEditTime();

	if (idx < mExecOutputs.size()) { mExecOutputs.erase(mExecOutputs.begin() + idx); }
	updateExits();
}

void GraphFunction::renameExecOutput(size_t idx, std::string name) {
	// invalidate the cache
	module().updateLastEditTime();

	if (idx < mExecOutputs.size()) { mExecOutputs[idx] = std::move(name); }
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

NamedDataType GraphFunction::localVariableFromName(boost::string_view name) const {
	for (auto local : mLocalVariables) {
		if (local.name == name) { return local; }
	}

	return {};
}

NamedDataType GraphFunction::getOrCreateLocalVariable(std::string name, DataType type,
                                                      bool* inserted) {
	auto local = localVariableFromName(name);

	if (local.valid()) {
		if (inserted != nullptr) { *inserted = false; }
		return local;
	}

	// invalidate the cache
	module().updateLastEditTime();

	mLocalVariables.emplace_back(name, type);
	if (inserted != nullptr) { *inserted = true; }

	return mLocalVariables[mLocalVariables.size() - 1];
}

bool GraphFunction::removeLocalVariable(boost::string_view name) {
	auto iter = std::find_if(mLocalVariables.begin(), mLocalVariables.end(),
	                         [&](auto& toTest) { return toTest.name == name; });

	bool erased = false;
	if (iter != mLocalVariables.end()) {
		mLocalVariables.erase(iter);
		erased = true;
	}
	if (!erased) { return false; }

	// invalidate the cache
	module().updateLastEditTime();

	// remove set and get nodes
	auto setNodes = nodesWithType(module().fullName(), "_set_" + name.to_string());
	for (const auto& node : setNodes) { removeNode(*node); }
	auto             getNodes = nodesWithType(module().fullName(), "_get_" + name.to_string());
	for (const auto& node : getNodes) { removeNode(*node); }

	return true;
}

void GraphFunction::renameLocalVariable(std::string oldName, std::string newName) {
	// invalidate the cache
	module().updateLastEditTime();

	bool setanything = false;
	for (auto& var : mLocalVariables) {
		if (var.name == oldName) {
			var.name = newName;

			setanything = true;
			break;
		}
	}

	// if it wans't found, then we don't have to update
	if (!setanything) { return; }

	// update existing nodes
	auto setNodes = nodesWithType(module().fullName(), "_set_" + oldName);
	for (const auto& node : setNodes) {
		// create a new node type
		std::unique_ptr<NodeType> ty;

		auto res = module().nodeTypeFromName("_set_" + newName, node->type().toJSON(), &ty);
		assert(!!res);

		node->setType(std::move(ty));
	}

	auto getNodes = nodesWithType(module().fullName(), "_get_" + oldName);
	for (const auto& node : getNodes) {
		// create a new node type
		std::unique_ptr<NodeType> ty;

		auto res = module().nodeTypeFromName("_get_" + newName, node->type().toJSON(), &ty);
		assert(!!res);

		node->setType(std::move(ty));
	}
}

void GraphFunction::retypeLocalVariable(boost::string_view name, DataType newType) {
	// invalidate the cache
	module().updateLastEditTime();

	std::string qualifiedName = newType.qualifiedName();

	for (auto& var : mLocalVariables) {
		if (var.name == name) {
			var.type = std::move(newType);
			break;
		}
	}

	// update existing nodes
	auto setNodes = nodesWithType(module().fullName(), "_set_" + name.to_string());
	for (const auto& node : setNodes) {
		// create a new node type
		std::unique_ptr<NodeType> ty;

		auto res = module().nodeTypeFromName("_set_" + name.to_string(), qualifiedName, &ty);
		assert(!!res);

		node->setType(std::move(ty));
	}

	auto getNodes = nodesWithType(module().fullName(), "_get_" + name.to_string());
	for (const auto& node : getNodes) {
		// create a new node type
		std::unique_ptr<NodeType> ty;

		auto res = module().nodeTypeFromName("_get_" + name.to_string(), qualifiedName, &ty);
		assert(!!res);

		node->setType(std::move(ty));
	}
}

std::vector<NodeInstance*> GraphFunction::setName(boost::string_view newName,
                                                  bool               updateReferences) {
	// invalidate the cache
	module().updateLastEditTime();

	auto oldName = mName;
	mName        = newName.to_string();

	if (updateReferences) {
		auto toUpdate = context().findInstancesOfType(module().fullName(), oldName);

		for (auto node : toUpdate) {
			std::unique_ptr<NodeType> ty;
			auto res = context().nodeTypeFromModule(module().fullName(), name(), {}, &ty);
			if (!res) { return {}; }

			node->setType(std::move(ty));
		}

		return toUpdate;
	}
	return {};
}

std::string GraphFunction::qualifiedName() const { return module().fullName() + ":" + name(); }

}  // namespace chi
