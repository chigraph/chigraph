/// \file ClangFinder.hpp
#ifndef CHI_CLANG_FINDER_HPP
#define CHI_CLANG_FINDER_HPP

#pragma once

#include <boost/filesystem/path.hpp>

namespace chi {
boost::filesystem::path findClang();
}

#endif  // CHI_CLANG_FINDER_HPP