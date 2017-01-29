#include "chig/GraphStruct.hpp"
#include "chig/Context.hpp"
#include "chig/GraphModule.hpp"

#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/DebugInfoMetadata.h>
#include <llvm/IR/DIBuilder.h>

namespace chig {

GraphStruct::GraphStruct(GraphModule& mod, std::string name)
    : mModule{&mod}, mContext{&mod.context()}, mName{name} {}

void GraphStruct::addType(DataType ty, std::string name, size_t addBefore) {
	Expects(addBefore <= nodes.size() && ty.valid() && !name.empty);
	
	
	mTypes.emplace_back(name, ty);
	
	// invalidate the current DataType
	mDataType = {};
}

void GraphStruct::modifyType(size_t id, DataType newTy, std::string newName) {
	Expects(id < nodes.size() && newTy.valid() && !newName.empty());
	
	mTypes[id] = {newName, newTy};
	
	// invalidate the current DataType
	mDataType = {};
}

void GraphStruct::removeType(size_t id) {
	Expects(id < nodes.size());
	
	mTypes.erase(mTypes.begin() + id);

	// invalidate the current DataType
	mDataType = {};
}


DataType GraphStruct::dataType() const {
	
	// if we have already calculated this, use that
	if(mDataType.valid()) {
		return mDataType;
	}
	
	// create llvm::Type
	std::vector<llvm::Type*> types; 
	types.reserve(types().size());
	for(const auto& type : types()) {
		types.push_back(type.second.llvmType());
	}
	auto llType = llvm::StructType::create(types, name());
	
	
	// create debug type
	llvm::DIBuilder dbuilder;
	
	for(const auto& type : types()) {
		dbuilder.createMemberType(nullptr, name(), nullptr, 0, type.debugType()->getSizeInBits(), )
	}
	
}


} // namespace chig
