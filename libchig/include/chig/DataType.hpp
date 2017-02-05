/// \file chig/DataType.hpp
/// Defines the DataType class

#pragma once

#ifndef CHIG_DATA_TYPE_HPP
#define CHIG_DATA_TYPE_HPP

#include "chig/ChigModule.hpp"

#include <llvm/IR/Type.h>

namespace chig {
/// A type of data
/// Loose wrapper around llvm::Type*, except it knows which ChigModule its in
struct DataType {
	/// Constructor
	/// \param chigMod The module
	/// \param typeName The ID of the type in the module
	/// \param llvmtype The underlying type
	DataType(ChigModule* chigMod = nullptr, std::string typeName = {},
	         llvm::Type* llvmtype = nullptr, llvm::DIType* debugTy = nullptr)
	    : mModule(chigMod), mName{typeName}, mLLVMType{llvmtype}, mDIType{debugTy} {}

	/// Get the module this is a part of
	/// \return The module
	ChigModule& module() const { return *mModule; }
	/// Get the unqualified name of the type
	/// \return The unqualified name
	std::string unqualifiedName() const { return mName; }
	/// Get the qualified name of the type (with the module)
	/// \return The qualified name
	std::string qualifiedName() const { return mModule->name() + ":" + mName; }
	/// Get the underlying \c llvm::Type
	/// \return the \c llvm::Type
	llvm::Type* llvmType() const { return mLLVMType; }
	/// Get the debug type
	/// \return The debug type
	llvm::DIType* debugType() const { return mDIType; }
	/// Check if the DataType is valid
	bool valid() const {
		return mModule != nullptr && mName != "" && mLLVMType != nullptr && mDIType != nullptr;
	}
	/// Equality check
	/// \param other The DataType to check equality against
	/// \return If they are equal
	bool operator==(const DataType& other) const {
		return mModule == other.mModule && mName == other.mName && mLLVMType == other.mLLVMType &&
		       mDIType == other.mDIType;
	}

	/// Inequality check
	/// \param other The DataType to check equality against
	/// \return If they are inequal
	bool operator!=(const DataType& other) const { return !(*this == other); }

private:
	ChigModule*   mModule;
	std::string   mName;
	llvm::Type*   mLLVMType;
	llvm::DIType* mDIType;
};

struct NamedDataType {
	NamedDataType(std::string n = {}, DataType ty = {}) : name{std::move(n)}, type{std::move(ty)} {}

	bool operator==(const NamedDataType& other) const {
		return name == other.name && type == other.type;
	}
	bool operator!=(const NamedDataType& other) const { return !(*this == other); }

	bool valid() const { return type.valid(); }

	std::string name;
	DataType    type;
};
}

#endif  // CHIG_DATA_TYPE_HPP
