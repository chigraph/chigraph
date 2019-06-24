#ifndef CHI_SUPPORT_FIND_PROGRAM_HPP
#define CHI_SUPPORT_FIND_PROGRAM_HPP

#include <filesystem>

namespace chi {

std::filesystem::path findProgram(const char* name);

}

#endif  //  CHI_SUPPORT_FIND_PROGRAM_HPP