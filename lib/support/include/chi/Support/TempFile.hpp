/// \file TmpFile.hpp

#ifndef CHI_SUPPORT_TMP_FILE_HPP
#define CHI_SUPPORT_TMP_FILE_HPP

#pragma once

#include <filesystem>
#include <string_view>

namespace chi {

std::filesystem::path makeTempPath(std::string_view extension = "");

}  // namespace chi

#endif  // CHI_SUPPORT_TMP_FILE_HPP
