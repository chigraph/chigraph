/// \file DataType.cpp

#include "chi/DataType.hpp"
#include "chi/ChiModule.hpp"

namespace chi {

std::string DataType::qualifiedName() const {
	return module().fullName() + ":" + unqualifiedName();
}

}  // namespace chi
