/// \file ExecutablePath.hpp

#pragma once

#ifndef CHI_SUPPORT_EXECUTABLE_PATH_HPP
#define CHI_SUPPORT_EXECUTABLE_PATH_HPP

#include <filesystem>

namespace chi {

/// Get the path of the executable
/// \return The exectuable path
std::filesystem::path executablePath();
}  // namespace chi

#endif  // CHI_SUPPORT_EXECUTABLE_PATH_HPP
