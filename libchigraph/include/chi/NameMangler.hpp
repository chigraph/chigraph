/// \file chi/NameMangler.hpp
/// Definitions for mangling functions

#pragma once

#ifndef CHI_NAME_MANGLER_HPP
#define CHI_NAME_MANGLER_HPP

#include <string>
#include <utility>

namespace chi {

/// \name Name Mangling
/// \brief Functions for mangling names into names used natively
/// \{

/// Mangle a function name
/// \param fullModuleName The full name of the module. Example github.com/russelltg/test/lib
/// \param name The name of the function
/// \return The mangled name
std::string mangleFunctionName(std::string fullModuleName, const std::string& name);

/// Unmangle a function name
/// \param mangled The mangled name
/// \return The unmangled name; {moduleName, functionName}
std::pair<std::string, std::string> unmangleFunctionName(std::string mangled);

/// \}
}  // namespace chi

#endif  // CHI_NAME_MANGLER_HPP
