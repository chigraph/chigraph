#pragma once

#ifndef CHIG_DATA_TYPE_HPP
#define CHIG_DATA_TYPE_HPP

#include "chig/ChigModule.hpp"
#include "chig/Fwd.hpp"

#include <llvm/IR/Type.h>

namespace chig
{
struct DataType {
	DataType(
		const ChigModule* mod = nullptr, std::string typeName = {}, llvm::Type* llvmtype = nullptr)
		: module{mod}, name{typeName}, lltype{llvmtype}
	{
	}

	const ChigModule* getModule() const { return module; }
	std::string getUnqualifiedName() const { return name; }
	std::string getQualifiedName() const { return module->name() + ":" + name; }
	llvm::Type* getLLVMType() const { return lltype; }
	bool isValid() const { return module != nullptr && name != "" && lltype != nullptr; }
	bool operator==(const DataType& other) const
	{
		return module == other.module && name == other.name && lltype == other.lltype;
	}

	bool operator!=(const DataType& other) const { return !(*this == other); }
private:
	const ChigModule* module;
	std::string name;
	llvm::Type* lltype;
};
}

#endif  // CHIG_DATA_TYPE_HPP
