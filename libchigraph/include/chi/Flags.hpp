#pragma once

#ifndef CHI_FLAGS_HPP
#define CHI_FLAGS_HPP

#include <type_traits>

namespace chi {

/// A template class for type-safe flags
///
/// Usage:
/// ```
/// enum class MyOpts {
///     First = 1u,
///     Second = 1u << 1,
///     FirstAndSecond = First | Second,
/// };
///
/// chi::Flags<MyOpts> fl{MyOpts::First | MyOpts::Second};
/// assert(fl & MyOpts::First);
/// ```
template <typename Enum>
struct Flags {
	static_assert(std::is_enum<Enum>::value, "Template argument to flags must be an enum");

	Flags() = default;
	Flags(Enum e) : mData{static_cast<decltype(mData)>(e)} {}

	Flags(const Flags& fl) = default;
	Flags(Flags&& fl)      = default;

	Flags& operator=(const Flags&) = default;
	Flags& operator=(Flags&&) = default;

	bool operator==(const Flags& rhs) const { return mData == rhs.mData; }

	bool operator!() const { return mData == 0; }

	explicit operator bool() const { return mData != 0; }

	Flags operator|(const Flags& rhs) const { return Flags{mData | rhs.mData}; }

	Flags operator&(const Flags& rhs) const { return Flags{mData & rhs.mData}; }

	Flags& operator|=(const Flags& rhs) {
		mData |= rhs.mData;
		return *this;
	}

	Flags& operator&=(const Flags& rhs) {
		mData &= rhs.mData;
		return *this;
	}

private:
	Flags(typename std::underlying_type<Enum>::type data) : mData{data} {}

	typename std::underlying_type<Enum>::type mData = 0;
};

}  // namespace chi

#endif  // CHI_FLAGS_HPP
