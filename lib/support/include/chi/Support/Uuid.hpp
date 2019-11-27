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
	Uuid() noexcept = default;
	template <typename InputIterator>
	Uuid(InputIterator begin, InputIterator end) noexcept {
		std::copy(begin, end, mData.begin());
	}
	~Uuid() noexcept = default;

	static Uuid                random() noexcept;
	static std::optional<Uuid> fromString(std::string_view from) noexcept;

	const std::array<uint8_t, 16>& data() const noexcept { return mData; }

	std::string toString() const noexcept;
	size_t      hash() const noexcept;
	bool        valid() const noexcept;

private:
	std::array<uint8_t, 16> mData = {};
};

inline bool operator==(const Uuid& lhs, const Uuid& rhs) noexcept {
	return lhs.data() == rhs.data();
}
inline bool operator!=(const Uuid& lhs, const Uuid& rhs) noexcept {
	return lhs.data() != rhs.data();
}
inline bool operator<(const Uuid& lhs, const Uuid& rhs) noexcept { return lhs.data() < rhs.data(); }
inline bool operator<=(const Uuid& lhs, const Uuid& rhs) noexcept {
	return lhs.data() <= rhs.data();
}
inline bool operator>(const Uuid& lhs, const Uuid& rhs) noexcept { return lhs.data() > rhs.data(); }
inline bool operator>=(const Uuid& lhs, const Uuid& rhs) noexcept {
	return lhs.data() >= rhs.data();
}

}  // namespace chi

// std::hash
namespace std {
template <>
struct hash<chi::Uuid> {
	size_t operator()(const chi::Uuid& uuid) const noexcept { return uuid.hash(); }
};
}  // namespace std