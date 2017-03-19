#ifndef CHI_HASH_UUID
#define CHI_HASH_UUID

#include <boost/uuid/uuid.hpp>

#include <functional>

namespace std {

template <>
struct hash<boost::uuids::uuid> {
	size_t operator()(const boost::uuids::uuid& toHash) const {
		return boost::uuids::hash_value(toHash);
	}
};
}

#endif  // CHI_HASH_UUID
