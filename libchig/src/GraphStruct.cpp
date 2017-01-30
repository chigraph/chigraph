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
	Expects(addBefore <= types().size() && ty.valid() && !name.empty());
	
	
	mTypes.emplace_back(name, ty);
	
	// invalidate the current DataType
	mDataType = {};
}

void GraphStruct::modifyType(size_t id, DataType newTy, std::string newName) {
	Expects(id < types().size() && newTy.valid() && !newName.empty());
	
	mTypes[id] = {newName, newTy};
	
	// invalidate the current DataType
	mDataType = {};
}

void GraphStruct::removeType(size_t id) {
	Expects(id < types().size());
	
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
	std::vector<llvm::Type*> llTypes; 
	llTypes.reserve(types().size());
	for(const auto& type : types()) {
		llTypes.push_back(type.second.llvmType());
	}
	auto llType = llvm::StructType::create(llTypes, name());
	
	
	// create debug type
	
	
}


} // namespace chig
