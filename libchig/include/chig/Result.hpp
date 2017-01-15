#ifndef CHIG_RESULT_HPP
#define CHIG_RESULT_HPP

#pragma once

#include "chig/ToString.hpp"
#include "chig/json.hpp"

#include <gsl/gsl>

namespace chig {
/// The result object, used for identifiying errors with good diagnostics
struct Result {
	/// Default constructor; defaults to success
	Result() : result_json(nlohmann::json::array()), success{true} {}
	/// Add a entry to the result, either a warning or an error
	/// \param ec The error/warning code. If it starts with E, then it is an error and success is
	/// set to false, if it starts with a W it's a warning and success can stay true if it is still
	/// true.
	/// \param overview Basic overview of the error, this shouldn't change based on the instance of
	/// the error
	/// \param data The detailed description this instance of the error
	void addEntry(const char* ec, const char* overview, nlohmann::json data) {
		Expects(ec[0] == 'E' || ec[0] == 'I' || ec[0] == 'W');

		result_json.push_back(
			nlohmann::json({{"errorcode", ec}, {"overview", overview}, {"data", data}}));
		if (ec[0] == 'E') success = false;
	}

	/// The result JSON
	nlohmann::json result_json;

	/// if it is successful
	bool success;

	/// Success test
	operator bool() { return success; }
	/// !Success test
	bool operator!() { return !success; }
	/// Dump to a pretty-printed error message
	std::string dump() const;
};

/// Append two Result objects
/// \param lhs The left error
/// \param rhs The right error
/// \return The concatinated errors
inline Result operator+(const Result& lhs, const Result& rhs) {
	Result ret;
	ret.success = !(!lhs.success || !rhs.success);  // if either of them are false, then result is

	// copy each of the results in
	std::copy(lhs.result_json.begin(), lhs.result_json.end(), std::back_inserter(ret.result_json));
	std::copy(rhs.result_json.begin(), rhs.result_json.end(), std::back_inserter(ret.result_json));

	return ret;
}

/// Append one result to an existing one
/// \param lhs The existing Result to add to
/// \param rhs The (usually temporary) result to be added into lhs
/// \return *this
inline Result& operator+=(Result& lhs, const Result& rhs) {
	lhs.success = !(!lhs.success || !rhs.success);  // if either of them are false, then result is

	// copy each of the results in
	std::copy(rhs.result_json.begin(), rhs.result_json.end(), std::back_inserter(lhs.result_json));

	return lhs;
}

/// Stream operator
/// \param lhs The stream
/// \param rhs The Result to print to lhs
/// \return lhs after printing
inline std::ostream& operator<<(std::ostream& lhs, const Result& rhs) {
	lhs << rhs.dump();

	return lhs;
}
}

#endif  // CHIG_RESULT_HPP
