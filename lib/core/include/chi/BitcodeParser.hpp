/// \file BitcodeParser.hpp

#pragma once

#ifndef CHI_BITCODE_PARSER_HPP
#define CHI_BITCODE_PARSER_HPP

#include "chi/Fwd.hpp"
#include "chi/Owned.hpp"

#include <filesystem>

namespace chi {

/// Parse a bitcode file. Convenience function to make it LLVM version independent and reutrn a
/// result
Result parseBitcodeFile(const std::filesystem::path& file, LLVMContextRef ctx,
                        OwnedLLVMModule* toFill);
Result parseBitcodeString(const std::string& bitcode, LLVMContextRef ctx, OwnedLLVMModule* toFill);
}  // namespace chi

#endif  // CHI_BITCODE_PARSER_HPP
