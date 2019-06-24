/// \file CCompiler.hpp

#ifndef CHI_C_COMPILER_HPP
#define CHI_C_COMPILER_HPP

#include <filesystem>
#include <string_view>
#include <vector>

#include "chi/Fwd.hpp"
#include "chi/Owned.hpp"

namespace chi {

/// Use clang to compile C source code to a llvm module
/// It also uses `stdCIncludePaths` to find basic include paths.
/// \param[in] clangPath The path to the `clang` executable.
/// \param[in] llvmContext The LLVM context to create the module into
/// \param[in] arguments The arguments to clang. Can include input files if desired.
/// \param[in] inputCCode The C code to compile. If `arguments` contains input files, then this can
/// be empty.
/// \param[out] toFill The unique pointer module to create the module inside
/// \return The Result
Result compileCToLLVM(const std::filesystem::path& clangPath, LLVMContextRef llvmContext,
                      std::vector<std::string> arguments, std::string_view inputCCode,
                      OwnedLLVMModule* toFill);

}  // namespace chi

#endif  // CHI_C_COMPILER_HPP
