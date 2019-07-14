/// \file Uuid.cpp

#include "chi/Support/Uuid.hpp"

#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <numeric>

namespace chi {

Uuid Uuid::random() {
	auto uuid = boost::uuids::random_generator()();
	Uuid ret;
	std::copy(uuid.data, uuid.data + sizeof(uuid.data), ret.mData.begin());
	return ret;
}
std::optional<Uuid> Uuid::fromString(std::string_view from) {
	auto uuid = boost::uuids::string_generator()(from.data());
	return Uuid{uuid.data, uuid.data + sizeof(uuid.data)};
}
std::string Uuid::toString() const {
	boost::uuids::uuid uuid;
	std::copy(mData.begin(), mData.end(), uuid.data);
	return boost::uuids::to_string(uuid);
}
size_t Uuid::hash() const {
	boost::uuids::uuid uuid;
	std::copy(mData.begin(), mData.end(), uuid.data);
	return boost::uuids::hash_value(uuid);
}
bool Uuid::valid() const { return std::accumulate(mData.begin(), mData.end(), 0) != 0; }
}  // namespace chi
