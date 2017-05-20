#pragma once

#ifndef CHI_HASH_FILESYSTEM_PATH_HPP
#define CHI_HASH_FILESYSTEM_PATH_HPP

#include <boost/filesystem.hpp>
#include <functional>

namespace std {

/// Hasing for filesystem paths so they can be used with unordered_* containers
template <>
struct hash<boost::filesystem::path> {
	
	/// The hash fucntion
	/// \param p The path to hash
	/// \return The hash
	size_t operator()(const boost::filesystem::path& p) const {
		return std::hash<std::string>()(p.generic_string());
	}
};
}

#endif  // CHI_HASH_FILESYSTEM_PATH_HPP
