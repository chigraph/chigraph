/// \file chi/Support/LibCLocator.hpp

#pragma once

#ifndef CHI_LIBCLOCATOR_HPP
#define CHI_LIBCLOCATOR_HPP

#include <boost/filesystem.hpp>

#include "Fwd.hpp"

namespace chi {

/// \name Standard include finder
/// \{

/// Gets the location of the standard C library to include
Result stdCIncludePaths(std::vector<boost::filesystem::path>& toFill);

/// \}
}

#endif  // CHI_LIBCLOCATOR_HPP
