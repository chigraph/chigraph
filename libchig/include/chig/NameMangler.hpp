/// \file chig/NameMangler.hpp
/// Definitions for mangling functions

#pragma once

#ifndef CHIG_NAME_MANGLER_HPP
#define CHIG_NAME_MANGLER_HPP

#include <string>
#include <utility>

#include <gsl/gsl>

namespace chig {

/// \name Name Mangling
/// \brief Functions for mangling names into names used natively
/// \{

/// Mangle a function name
/// \param fullModuleName The full name of the module. Example github.com/russelltg/test/lib
/// \param name The name of the function
/// \return The mangled name
std::string mangleFunctionName(gsl::cstring_span<> fullModuleName, gsl::cstring_span<> name);

/// Unmangle a function name
/// \param mangledName The mangled name
/// \return The unmangled name
std::pair<std::string, std::string> unmangleFunctionName(gsl::cstring_span<> mangledName);

/// \}
}

#endif  // CHIG_NAME_MANGLER_HPP
