/// \file chi/DataType.hpp
/// Defines the DataType class

#pragma once

#ifndef CHI_DATA_TYPE_HPP
#define CHI_DATA_TYPE_HPP

#include "chi/Fwd.hpp"

#include <string>

namespace chi {
/// A type of data
/// Loose wrapper around llvm::Type*, except it knows which ChiModule it's in and it embeds debug
/// types
struct DataType {
	/// Constructor
	/// \param chiMod The module
	/// \param typeName The ID of the type in the module
	/// \param llvmtype The underlying type
	/// \param debugTy The debug type for the DataType
	/// \param reference if the type is a reference (refcounted)
	DataType(ChiModule* chiMod = nullptr, std::string typeName = {}, llvm::Type* llvmtype = nullptr,
	         llvm::DIType* debugTy = nullptr, bool reference = false) noexcept
	    : mModule(chiMod), mName{typeName}, mLLVMType{llvmtype}, mDIType{debugTy}, mReference{reference} {}

	/// Get the module this is a part of
	/// \return The module
	ChiModule& module() const noexcept { return *mModule; }
	/// Get the unqualified name of the type
	/// \return The unqualified name
	const std::string& unqualifiedName() const noexcept { return mName; }
	/// Get the qualified name of the type (module().fullName() + ":" name())
	/// \return The qualified name
	std::string qualifiedName() const noexcept;
	/// Get the underlying \c llvm::Type
	/// \return the \c llvm::Type
	llvm::Type* llvmType() const noexcept { return mLLVMType; }
	/// Get the debug type
	/// \return The debug type
	llvm::DIType* debugType() const noexcept { return mDIType; }
	/// See if this type is a refernence
	/// \return If it's a referencne
	bool reference() const noexcept { return mReference; }
	/// Check if the DataType is valid (if it's actually bound to a type and module)
	/// \return `true` if valid, `false` otherwise
	bool valid() const noexcept {
		return mModule != nullptr && mName != "" && mLLVMType != nullptr && mDIType != nullptr;
	}

private:
	ChiModule*    mModule;
	std::string   mName;
	llvm::Type*   mLLVMType;
	llvm::DIType* mDIType;
	bool          mReference;
};

/// Equality check
/// \param lhs The first DataType
/// \param rhs The DataType to check equality against
/// \return If they are equal
/// \relates DataType
inline bool operator==(const DataType& lhs, const DataType& rhs) {
	return &lhs.module() == &rhs.module() && lhs.unqualifiedName() == rhs.unqualifiedName();
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
	DataType type;
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
}  // namespace chi

#endif  // CHI_DATA_TYPE_HPP
