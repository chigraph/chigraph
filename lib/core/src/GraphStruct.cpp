/// \file GraphStruct.cpp

#include "chi/GraphStruct.hpp"

#include "chi/Context.hpp"
#include "chi/FunctionCompiler.hpp"
#include "chi/GraphModule.hpp"
#include "chi/NodeInstance.hpp"
#include "chi/NodeType.hpp"
#include "chi/Support/Result.hpp"

namespace chi {

GraphStruct::GraphStruct(GraphModule& mod, std::string name)
    : mModule{&mod}, mContext{&mod.context()}, mName{std::move(name)} {}

std::vector<NodeInstance*> GraphStruct::setName(std::string newName, bool updateReferences) {
	assert(!newName.empty() && "Cannot pass an empty name to GraphStruct::setName");

	module().updateLastEditTime();

	auto oldName = name();
	mName        = std::move(newName);

	// find references to update
	if (updateReferences) {
		auto makeInstances =
		    context().findInstancesOfType(module().fullNamePath(), "_make_" + oldName);

		for (auto makeInst : makeInstances) {
			std::unique_ptr<NodeType> type;
			auto                      res = module().nodeTypeFromName("_make_" + name(), {}, &type);

			if (!res) { return {}; }

			makeInst->setType(std::move(type));
		}

		auto breakInstances =
		    context().findInstancesOfType(module().fullNamePath(), "_break_" + oldName);

		for (auto breakInst : breakInstances) {
			std::unique_ptr<NodeType> type;
			auto res = module().nodeTypeFromName("_break_" + name(), {}, &type);

			if (!res) { return {}; }

			breakInst->setType(std::move(type));
		}

		// append breakInstances to makeInstances so wer can return the updated nodes
		makeInstances.reserve(breakInstances.size() + makeInstances.size());
		std::copy(breakInstances.begin(), breakInstances.end(), std::back_inserter(makeInstances));

		return makeInstances;
	}

	return {};
}

void GraphStruct::addType(DataType ty, std::string name, size_t addBefore, bool updateReferences) {
	assert(addBefore <= types().size() && ty.valid());

	// invalidate the cache
	module().updateLastEditTime();

	mTypes.emplace_back(name, ty);

	// invalidate the current DataType
	mDataType = {};

	if (updateReferences) { updateNodeReferences(); }
}

void GraphStruct::modifyType(size_t id, DataType newTy, std::string newName,
                             bool updateReferences) {
	assert(id < types().size() && newTy.valid() && !newName.empty());

	// invalidate the cache
	module().updateLastEditTime();

	mTypes[id] = {std::move(newName), std::move(newTy)};

	// invalidate the current DataType
	mDataType = {};

	if (updateReferences) { updateNodeReferences(); }
}

void GraphStruct::removeType(size_t id, bool updateReferences) {
	assert(id < types().size());

	// invalidate the cache
	module().updateLastEditTime();

	mTypes.erase(mTypes.begin() + id);

	// invalidate the current DataType
	mDataType = {};

	if (updateReferences) { updateNodeReferences(); }
}

DataType GraphStruct::dataType() {
	// if we have already calculated this, use that
	if (mDataType.valid()) { return mDataType; }

	if (types().empty()) { return {}; }

	// create llvm::Type
	std::vector<LLVMTypeRef> llTypes;
	llTypes.reserve(types().size());

	for (const auto& type : types()) { llTypes.push_back(type.type.llvmType()); }

	auto llType = LLVMStructType(llTypes.data(), llTypes.size(), false);

	mDataType = DataType(&module(), name(), llType);

	return mDataType;
}

LLVMMetadataRef GraphStruct::debugType(FunctionCompiler& compiler) {
	if (mDebugType != nullptr) { return mDebugType; }

	std::vector<LLVMMetadataRef> diTypes;
	diTypes.reserve(types().size());

	size_t currentOffset = 0;

	for (const auto& type : types()) {
		auto debugType = type.type.debugType(compiler);

		auto member = LLVMDIBuilderCreateMemberType(
		    compiler.diBuilder(), compiler.debugFile(), type.name.c_str(), type.name.size(),
		    compiler.debugFile(), 0, LLVMDITypeGetSizeInBits(debugType), 0, currentOffset,
		    LLVMDIFlagZero, nullptr);

		diTypes.push_back(member);

		currentOffset += LLVMDITypeGetSizeInBits(debugType);
	}

	mDebugType = LLVMDIBuilderCreateStructType(
	    compiler.diBuilder(), compiler.debugFile(), name().c_str(), name().size(),
	    compiler.debugFile(), 0, currentOffset, 0, LLVMDIFlagZero, nullptr, diTypes.data(),
	    diTypes.size(), 0, nullptr, dataType().qualifiedName().c_str(),
	    dataType().qualifiedName().length());

	return mDebugType;
}

void GraphStruct::updateNodeReferences() {
	auto makeInstances = context().findInstancesOfType(module().fullNamePath(), "_make_" + name());

	for (auto inst : makeInstances) {
		// make a make type
		std::unique_ptr<NodeType> ty;
		auto                      res = module().nodeTypeFromName("_make_" + name(), {}, &ty);
		if (!res) { return; }

		inst->setType(std::move(ty));
	}

	auto breakInstances =
	    context().findInstancesOfType(module().fullNamePath(), "_break_" + name());
	for (auto inst : breakInstances) {
		// make a break type
		std::unique_ptr<NodeType> ty;
		auto                      res = module().nodeTypeFromName("_break_" + name(), {}, &ty);
		if (!res) { return; }

		inst->setType(std::move(ty));
	}
}

}  // namespace chi
