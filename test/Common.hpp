#include "catch.hpp"

#include <chi/Core/DataType.hpp>

namespace Catch {
std::string toString(const chi::DataType& ty) { return ty.qualifiedName(); }
std::string toString(const chi::NamedDataType& ty) {
	return "{" + ty.name + ", " + ty.type.qualifiedName() + "}";
}
}
