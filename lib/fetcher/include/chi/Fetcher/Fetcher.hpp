/// \file Fetcher.hpp

#ifndef CHI_FETCHER_HPP
#define CHI_FETCHER_HPP

#pragma once

#include <chi/Support/Result.hpp>
#include <filesystem>

namespace chi {

// Version control types
enum class VCSType {
	// Git
	Git,

	// TODO: implement more
	Unknown

};
/// Downloads a module from a remote URL, currently supports
///  - github
/// \param workspacePath The path to the workspace
/// \pre `fs::is_regular_file(workspacePath / ".chigraphworkspace")`
/// \param name The name of the module to fetch
/// \param recursive Should all dependencies be cloned as well?
/// \return The Result
Result fetchModule(const std::filesystem::path& workspacePath, const std::filesystem::path& name,
                   bool recursive);

/// Get the URL for a VCS repository from a module name.
/// \param path The module name
/// \return {The type of VCS that it is, the URL to clone, the relative path to clone to}
std::tuple<VCSType, std::string, std::string> resolveUrlFromModuleName(
    const std::filesystem::path& path);
}  // namespace chi

#endif  // CHI_FETCHER_HPP
