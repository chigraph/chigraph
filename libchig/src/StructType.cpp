#include "chig/StructType.hpp"
#include "chig/JsonModule.hpp"
#include "chig/Context.hpp"

namespace chig {


StructType::StructType(JsonModule& mod, std::vector<std::string, DataType> types) : mModule{&mod}, mContext{&mod.context()} {}


}
