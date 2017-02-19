#include <catch.hpp>

#include <chig/DataType.hpp>

namespace Catch {
std::string toString(const chig::DataType& ty) { return ty.qualifiedName(); }
std::string toString(const chig::NamedDataType& ty) {
	return "{" + ty.name + ", " + ty.type.qualifiedName() + "}";
}
}
