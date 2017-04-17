#include "chi/GraphStruct.hpp"
#include "chi/Context.hpp"
#include "chi/GraphModule.hpp"

#include <llvm/IR/DIBuilder.h>
#include <llvm/IR/DebugInfo.h>
#include <llvm/IR/DerivedTypes.h>

#if LLVM_VERSION_MAJOR <= 3 && LLVM_VERSION_MAJOR <= 6
#include <llvm/IR/Module.h>
#endif

namespace chi {

GraphStruct::GraphStruct(GraphModule& mod, std::string name)
    : mModule{&mod}, mContext{&mod.context()}, mName{std::move(name)} {}

void GraphStruct::addType(DataType ty, std::string name, size_t addBefore) {
	Expects(addBefore <= types().size() && ty.valid() && !name.empty());

	mTypes.emplace_back(name, ty);

	// invalidate the current DataType
	mDataType = {};
}

void GraphStruct::modifyType(size_t id, DataType newTy, std::string newName) {
	Expects(id < types().size() && newTy.valid() && !newName.empty());

	mTypes[id] = {std::move(newName), std::move(newTy)};

	// invalidate the current DataType
	mDataType = {};
}

void GraphStruct::removeType(size_t id) {
	Expects(id < types().size());

	mTypes.erase(mTypes.begin() + id);

	// invalidate the current DataType
	mDataType = {};
}

DataType GraphStruct::dataType() {
	// if we have already calculated this, use that
	if (mDataType.valid()) { return mDataType; }

	if (types().empty()) { return {}; }

	// create llvm::Type
	std::vector<llvm::Type*> llTypes;
	llTypes.reserve(types().size());

	std::vector<
#if LLVM_VERSION_MAJOR <= 3 && LLVM_VERSION_MINOR <= 6
		llvm::DIDescriptor
#else
		llvm::Metadata*
#endif
		> diTypes;
	diTypes.reserve(types().size());

	size_t currentOffset = 0;
	
	
#if LLVM_VERSION_MAJOR <= 3 && LLVM_VERSION_MINOR <= 6
	// make a temp module so we can make a DIBuilder
	auto tmpMod = std::make_unique<llvm::Module>("tmp", context().llvmContext());
	auto diBuilder = std::make_unique<llvm::DIBuilder>(*tmpMod);
#endif
	
	for (const auto& type : types()) {
		auto debugType = type.type.debugType();

		llTypes.push_back(type.type.llvmType());
		
	auto member = 
#if LLVM_VERSION_MAJOR <= 3 && LLVM_VERSION_MINOR <= 6
		diBuilder->createMemberType(llvm::DIDescriptor(), type.name, llvm::DIFile(), 0, debugType->getSizeInBits(), 8, currentOffset, 0, *debugType)
#else
		    llvm::DIDerivedType::get(context().llvmContext(), llvm::dwarf::DW_TAG_member,
#if LLVM_VERSION_MAJOR <= 3 && LLVM_VERSION_MINOR <= 8
		                             llvm::MDString::get(context().llvmContext(), type.name),
#else
		                             type.name,
#endif
		                             nullptr, 0, nullptr, debugType, debugType->getSizeInBits(), 8,
		                             currentOffset, llvm::DINode::DIFlags{}, nullptr)
#endif
			;
		diTypes.push_back(member);

		currentOffset += debugType->getSizeInBits();
	}
	auto llType = llvm::StructType::create(llTypes, name());

	auto diStructType = 
#if LLVM_VERSION_MAJOR <= 3 && LLVM_VERSION_MINOR <= 6
		new llvm::DICompositeType(diBuilder->createStructType(llvm::DIDescriptor(), name(), llvm::DIFile(), 0, currentOffset, 8, 0, llvm::DIType(), diTypes)); // TODO (#77): yeah this is a memory leak. Fix it.
#else
		llvm::DICompositeType::get(
			context().llvmContext(), llvm::dwarf::DW_TAG_structure_type, name(), nullptr, 0, nullptr,
			nullptr, currentOffset, 8, 0, llvm::DINode::DIFlags{},
			llvm::MDTuple::get(context().llvmContext(), diTypes), 0, nullptr, {}, "")
#endif
	;

	mDataType = DataType(&module(), name(), llType, diStructType);

	return mDataType;
}

}  // namespace chi
