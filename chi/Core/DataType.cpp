/// \file DataType.cpp

#include "DataType.hpp"
#include "ChiModule.hpp"

namespace chi {

std::string DataType::qualifiedName() const {
	return module().fullName() + ":" + unqualifiedName();
}

}  // namespace chi
