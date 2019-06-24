/// \file chi/Support/LibCLocator.hpp

#pragma once

#ifndef CHI_LIBCLOCATOR_HPP
#define CHI_LIBCLOCATOR_HPP

#include <filesystem>
#include <vector>

#include "chi/Support/Fwd.hpp"

namespace chi {

/// \name Standard include finder
/// \{

/// Gets the location of the standard C library to include
Result stdCIncludePaths(std::vector<std::filesystem::path>& toFill);

/// \}
}  // namespace chi

#endif  // CHI_LIBCLOCATOR_HPP
