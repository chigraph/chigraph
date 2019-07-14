/// \file Uuid.hpp

#pragma once

#include <array>
#include <cstdint>
#include <functional>
#include <optional>
#include <string>

namespace chi {

class Uuid {
public:
	Uuid() = default;
	template <typename InputIterator>
	Uuid(InputIterator begin, InputIterator end) {
		std::copy(begin, end, mData.begin());
	}
	~Uuid() = default;

	static Uuid                random();
	static std::optional<Uuid> fromString(std::string_view from);

	const std::array<uint8_t, 16>& data() const { return mData; }

	std::string toString() const;
	size_t      hash() const;
	bool        valid() const;

private:
	std::array<uint8_t, 16> mData = {};
};

inline bool operator==(const Uuid& lhs, const Uuid& rhs) { return lhs.data() == rhs.data(); }
inline bool operator!=(const Uuid& lhs, const Uuid& rhs) { return lhs.data() != rhs.data(); }
inline bool operator<(const Uuid& lhs, const Uuid& rhs) { return lhs.data() < rhs.data(); }
inline bool operator<=(const Uuid& lhs, const Uuid& rhs) { return lhs.data() <= rhs.data(); }
inline bool operator>(const Uuid& lhs, const Uuid& rhs) { return lhs.data() > rhs.data(); }
inline bool operator>=(const Uuid& lhs, const Uuid& rhs) { return lhs.data() >= rhs.data(); }

}  // namespace chi

// std::hash
namespace std {
template <>
struct hash<chi::Uuid> {
	size_t operator()(const chi::Uuid& uuid) const { return uuid.hash(); }
};
}  // namespace std