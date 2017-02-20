/// \file chig/DataType.hpp
/// Defines the DataType class

#pragma once

#ifndef CHIG_DATA_TYPE_HPP
#define CHIG_DATA_TYPE_HPP

#include "chig/ChigModule.hpp"

#include <llvm/IR/Type.h>

namespace chig {
/// A type of data
/// Loose wrapper around llvm::Type*, except it knows which ChigModule it's in and it embeds debug types
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
	const std::string& unqualifiedName() const { return mName; }
	/// Get the qualified name of the type (module().fullName() + ":" name())
	/// \return The qualified name
	std::string qualifiedName() const { return module().fullName() + ":" + unqualifiedName(); }
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

private:
	ChigModule*   mModule;
	std::string   mName;
	llvm::Type*   mLLVMType;
	llvm::DIType* mDIType;
};

/// Equality check
/// \param lhs The first DataType
/// \param rhs The DataType to check equality against
/// \return If they are equal
/// \relates DataType
inline bool operator==(const DataType& lhs, const DataType& rhs) {
	return &lhs.module() == &rhs.module() && lhs.unqualifiedName() == rhs.unqualifiedName() && lhs.llvmType() == rhs.llvmType() &&
			lhs.debugType() == rhs.debugType();
}

/// Inequality check
/// \param lhs The first DataType
/// \param rhs The DataType to check equality against
/// \return If they aren't equal
/// \relates DataType
inline bool operator!=(const DataType& lhs, const DataType& rhs) { return !(lhs == rhs); }


/// Basicaly a std::pair<std::string, DataType>, except it has nicer names.
struct NamedDataType {
	
	/// Construct a NamedDataType from the name and the type
	/// \param n The name
	/// \param ty The type
	NamedDataType(std::string n = {}, DataType ty = {}) : name{std::move(n)}, type{std::move(ty)} {}

	/// See if the pair is valid
	/// \return if it's valid
	bool valid() const { return type.valid(); }

	/// The name
	std::string name;
	
	/// The type
	DataType    type;
};

/// Check if two NamedDataType objects are equal
/// \param lhs The first type
/// \param rhs The type to compare to
/// \return if they are equal
/// \relates NamedDataType
inline bool operator==(const NamedDataType& lhs, const NamedDataType& rhs) {
	return lhs.name == rhs.name && lhs.type == rhs.type;
}

/// Check if two NamedDataType objects aren't equal
/// \param lhs The first type
/// \param rhs The type to compare to
/// \return if they aren't equal
/// \relates NamedDataType
inline bool operator!=(const NamedDataType& lhs, const NamedDataType& rhs) { return !(lhs == rhs); }


}

#endif  // CHIG_DATA_TYPE_HPP
