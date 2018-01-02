/// \file BitcodeParser.hpp

#pragma once

#ifndef CHI_BITCODE_PARSER_HPP
#define CHI_BITCODE_PARSER_HPP

#include "Fwd.hpp"

#include "chi/Support/Fwd.hpp"

#include <boost/filesystem/path.hpp>

namespace chi {

/// Parse a bitcode file. Convenience function to make it LLVM version independent and reutrn a
/// result
Result parseBitcodeFile(const boost::filesystem::path& file, llvm::LLVMContext& ctx,
                        std::unique_ptr<llvm::Module>* toFill);
Result parseBitcodeString(const std::string& bitcode, llvm::LLVMContext& ctx,
                          std::unique_ptr<llvm::Module>* toFill);
}

#endif  // CHI_BITCODE_PARSER_HPP
