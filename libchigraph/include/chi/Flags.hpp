/// \file Flags.hpp

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

	/// Default constructor
	Flags() = default;

	/// Constructor from an `Enum` type
	/// \param e The enum to copy from
	Flags(Enum e) : mData{static_cast<decltype(mData)>(e)} {}

	/// Copy constructor
	/// \param fl The Flags object to copy from
	Flags(const Flags& fl) = default;

	/// Move constructor
	/// \param fl The Flags object to move from
	Flags(Flags&& fl) = default;

	/// Copy assignment
	/// \param fl The Flags object to copy from
	/// \return `*this`
	Flags& operator=(const Flags& fl) = default;

	/// Move assignment
	/// \param The Flags object to move from
	/// \return `*this`
	Flags& operator=(Flags&& fl) = default;

	/// See if one Flags object is equal to the other
	/// \param rhs The Flags object to comare to
	/// \return if they are equal
	bool operator==(const Flags& rhs) const { return mData == rhs.mData; }

	/// See if one Flags object isn't equal to the other
	/// \return if they are equal
	bool operator!() const { return mData == 0; }

	/// Check if the flags object isn't set to zero
	/// Useful for use after a `&`
	/// \return true if it's not zero
	explicit operator bool() const { return mData != 0; }

	/// Bitwise OR
	/// \param rhs The Flags object to OR with
	/// \return The new Flags object
	Flags operator|(const Flags& rhs) const { return Flags{mData | rhs.mData}; }

	/// Bitwise AND
	/// \param rhs The Flags object to AND with
	/// \return The new Flags object
	Flags operator&(const Flags& rhs) const { return Flags{mData & rhs.mData}; }

	/// Bitwise OR assignment
	/// \param rhs The Flags obejct to OR `*this` with
	/// \return `*this`
	Flags& operator|=(const Flags& rhs) {
		mData |= rhs.mData;
		return *this;
	}

	/// Bitwise AND assignment
	/// \param rhs The Flags obejct to AND `*this` with
	/// \return `*this`
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
