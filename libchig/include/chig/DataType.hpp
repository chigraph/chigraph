#pragma once

#ifndef CHIG_DATA_TYPE_HPP
#define CHIG_DATA_TYPE_HPP

#include "chig/ChigModule.hpp"
#include "chig/Fwd.hpp"

#include <llvm/IR/Type.h>

namespace chig
{
/// A type of data
/// Loose wrapper around llvm::Type*, except it knows which ChigModule its in
struct DataType {
	/// Constructor
	/// \param mod The module
	/// \param typeName The ID of the type in the module
	/// \param llvmtype The underlying typ
	DataType(ChigModule* cMod = nullptr, std::string typeName = {}, llvm::Type* llvmtype = nullptr)
		: mModule(cMod), mName{typeName}, mLLVMType{llvmtype}
	{
	}

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
	/// Check if the DataType is valid
	bool valid() const { return mModule != nullptr && mName != "" && mLLVMType != nullptr; }
	/// Equality check
	/// \param other The DataType to check equality against
	/// \return If they are equal
	bool operator==(const DataType& other) const
	{
		return mModule == other.mModule && mName == other.mName && mLLVMType == other.mLLVMType;
	}

	/// Inequality check
	/// \param other The DataType to check equality against
	/// \return If they are inequal
	bool operator!=(const DataType& other) const { return !(*this == other); }
private:
	ChigModule* mModule;
	std::string mName;
	llvm::Type* mLLVMType;
};
}

#endif  // CHIG_DATA_TYPE_HPP
