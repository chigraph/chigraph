#pragma once

#ifndef CHIG_STRUCT_TYPE_HPP
#define CHIG_STRUCT_TYPE_HPP

#include <vector>
#include <string>

#include "chig/Fwd.hpp"
#include "chig/DataType.hpp"

namespace chig {

/// A class holding a compound type defined in a JsonModule
struct StructType {
	
	StructType(JsonModule& mod, std::vector<std::string, DataType> types);
	
	
	Context& context() const { return *mContext; }
	
	JsonModule& module() const {return *mModule;}
	
private:
	
	JsonModule* mModule;
	Context* mContext;
	
	std::vector<std::string, DataType> mTypes
};

}

#endif // CHIG_STRUCT_TYPE_HPP
