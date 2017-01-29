#include "chig/StructType.hpp"
#include "chig/GraphModule.hpp"
#include "chig/Context.hpp"

namespace chig {


StructType::StructType(GraphModule& mod, std::vector<std::string, DataType> types) : mModule{&mod}, mContext{&mod.context()} {}


}
