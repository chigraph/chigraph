#pragma once

#ifndef CHIG_NAME_MANGLER_HPP
#define CHIG_NAME_MANGLER_HPP

#include <string>
#include <utility>

#include <gsl/gsl>

namespace chig {
/// Mangle a function name
/// \param fullModuleName The full name of the module. Example github.com/russelltg/test/lib
/// \param name The name of the function
std::string mangleFunctionName(gsl::cstring_span<> fullModuleName, gsl::cstring_span<> name);

/// Unmangle a function name
/// \param mangledName The mangled name
std::pair<std::string, std::string> unmangleFunctionName(gsl::cstring_span<> mangledName);
}

#endif  // CHIG_NAME_MANGLER_HPP
