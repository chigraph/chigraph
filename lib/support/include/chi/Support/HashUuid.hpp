/// \file chi/Support/HashUuid.hpp

#pragma once

#ifndef CHI_HASH_UUID
#define CHI_HASH_UUID

#include <boost/uuid/uuid.hpp>
#include <functional>

namespace std {

/// Class so uuids can be used with unordered_* containers.
template <>
struct hash<boost::uuids::uuid> {
	/// The hash function
	/// \param toHash The UUID to hash
	/// \return The hash
	size_t operator()(const boost::uuids::uuid& toHash) const {
		return boost::uuids::hash_value(toHash);
	}
};
}

#endif  // CHI_HASH_UUID
