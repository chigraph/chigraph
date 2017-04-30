#pragma once

#ifndef CHI_LLVM_VERSION_HPP
#define CHI_LLVM_VERSION_HPP

#include <llvm/Config/llvm-config.h>

#define LLVM_VERSION_LESS_EQUAL(major, minor) ((LLVM_VERSION_MAJOR == major && LLVM_VERSION_MINOR <= minor) || (LLVM_VERSION_MAJOR < major))
#define LLVM_VERSION_AT_LEAST(major, minor) ((LLVM_VERSION_MAJOR == major && LLVM_VERSION >= minor) || (LLVM_VERSION_MAJOR > major))

#endif // CHI_LLVM_VERSION_HPP
