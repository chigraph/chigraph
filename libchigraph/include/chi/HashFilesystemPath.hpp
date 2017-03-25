#pragma once

#ifndef CHI_HASH_FILESYSTEM_PATH_HPP
#define CHI_HASH_FILESYSTEM_PATH_HPP

#include <boost/filesystem.hpp>
#include <functional>

namespace std {
template <>
struct hash<boost::filesystem::path> {
	size_t operator()(const boost::filesystem::path& p) const {
		return std::hash<std::string>()(p.generic_string());
	}
};
}

#endif  // CHI_HASH_FILESYSTEM_PATH_HPP
