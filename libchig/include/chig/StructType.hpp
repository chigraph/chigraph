#pragma once

#ifndef CHIG_STRUCT_TYPE_HPP
#define CHIG_STRUCT_TYPE_HPP

#include <string>
#include <vector>

#include "chig/DataType.hpp"
#include "chig/Fwd.hpp"

namespace chig {

/// A class holding a compound type defined in a GraphModule
struct StructType {
	StructType(GraphModule& mod, std::vector<std::pair<std::string, DataType>> types);

	Context& context() const { return *mContext; }

	GraphModule& module() const { return *mModule; }

private:
	GraphModule* mModule;
	Context*     mContext;

	std::vector<std::pair<std::string, DataType>> mTypes;
};
}

#endif  // CHIG_STRUCT_TYPE_HPP
