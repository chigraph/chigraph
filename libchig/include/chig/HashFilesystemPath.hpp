#pragma once

#ifndef CHIG_HASH_FILESYSTEM_PATH_HPP
#define CHIG_HASH_FILESYSTEM_PATH_HPP

#include <boost/filesystem.hpp>

namespace std {
template <>
struct hash<boost::filesystem::path> {
	size_t operator()(const boost::filesystem::path& p) const {
		return std::hash<std::string>()(p.generic_string());
	}
};
}

#endif  // CHIG_HASH_FILESYSTEM_PATH_HPP
