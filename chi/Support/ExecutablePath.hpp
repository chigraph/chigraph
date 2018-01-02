/// \file ExecutablePath.hpp

#pragma once

#ifndef CHI_SUPPORT_EXECUTABLE_PATH_HPP
#define CHI_SUPPORT_EXECUTABLE_PATH_HPP

#include <boost/filesystem/path.hpp>

namespace chi {

/// Get the path of the executable
/// \return The exectuable path
boost::filesystem::path executablePath();
}

#endif  // CHI_SUPPORT_EXECUTABLE_PATH_HPP
