/// \file GraphStruct.cpp

#include "chi/GraphStruct.hpp"
#include "chi/Context.hpp"
#include "chi/GraphModule.hpp"
#include "chi/LLVMVersion.hpp"
#include "chi/NodeInstance.hpp"
#include "chi/NodeType.hpp"
#include "chi/Result.hpp"

#include <llvm/IR/DIBuilder.h>
#include <llvm/IR/DebugInfo.h>
#include <llvm/IR/DerivedTypes.h>

#if LLVM_VERSION_LESS_EQUAL(3, 6)
#include <llvm/IR/Module.h>
#endif

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
	std::vector<llvm::Type*> llTypes;
	llTypes.reserve(types().size());

	std::vector<
#if LLVM_VERSION_LESS_EQUAL(3, 5)
	    llvm::Value*
#else
	    llvm::Metadata*
#endif
	    >
	    diTypes;
	diTypes.reserve(types().size());

	size_t currentOffset = 0;

#if LLVM_VERSION_LESS_EQUAL(3, 6)
	// make a temp module so we can make a DIBuilder
	auto tmpMod    = std::make_unique<llvm::Module>("tmp", context().llvmContext());
	auto diBuilder = std::make_unique<llvm::DIBuilder>(*tmpMod);
#endif

	for (const auto& type : types()) {
		auto debugType = type.type.debugType();

		llTypes.push_back(type.type.llvmType());

		auto member =
#if LLVM_VERSION_LESS_EQUAL(3, 6)
		    diBuilder->createMemberType(llvm::DIDescriptor(), type.name, llvm::DIFile(), 0,
		                                debugType->getSizeInBits(), 8, currentOffset, 0, *debugType)
#else
		    llvm::DIDerivedType::get(context().llvmContext(), llvm::dwarf::DW_TAG_member,
#	if LLVM_VERSION_LESS_EQUAL(3, 8)
		                             llvm::MDString::get(context().llvmContext(), type.name),
#	else
		                             type.name,
#	endif
		                             nullptr, 0, nullptr, debugType, debugType->getSizeInBits(), 8,
		                             currentOffset, 
#	if LLVM_VERSION_AT_LEAST(5, 0)
							   llvm::None,
#	endif
							   llvm::DINode::DIFlags{}, nullptr)
#endif
		    ;
		
		diTypes.push_back(member);

		currentOffset += debugType->getSizeInBits();
	}
	auto llType = llvm::StructType::create(llTypes, name());

	auto diStructType =
#if LLVM_VERSION_LESS_EQUAL(3, 6)
	    new llvm::DICompositeType(diBuilder->createStructType(
	        llvm::DIDescriptor(), name(), llvm::DIFile(), 0, currentOffset, 8, 0, llvm::DIType(),
	        diBuilder->getOrCreateArray(
	            diTypes)));  // TODO (#77): yeah this is a memory leak. Fix it.
#else
	    llvm::DICompositeType::get(
	        context().llvmContext(), llvm::dwarf::DW_TAG_structure_type, name(), nullptr, 0,
	        nullptr, nullptr, currentOffset, 8, 0, llvm::DINode::DIFlags{},
	        llvm::MDTuple::get(context().llvmContext(), diTypes), 0, nullptr, {}, "")
#endif
	;

	mDataType = DataType(&module(), name(), llType, diStructType);

	return mDataType;
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
