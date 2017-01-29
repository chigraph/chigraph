#include "chig/StructType.hpp"
#include "chig/Context.hpp"
#include "chig/GraphModule.hpp"

namespace chig {

StructType::StructType(GraphModule& mod, std::vector<std::pair<std::string, DataType>> types)
    : mModule{&mod}, mContext{&mod.context()} {}
}
