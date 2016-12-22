#ifndef CHIG_RESULT_HPP
#define CHIG_RESULT_HPP

#pragma once

#include "chig/json.hpp"

#include <gsl/gsl>

namespace chig
{
struct Result {
	Result() : result_json(nlohmann::json::array()), success{true} {}
	void addEntry(const char* ec, const char* overview, nlohmann::json data)
	{
		Expects(ec[0] == 'E' || ec[0] == 'I' || ec[0] == 'W');

		result_json.push_back(
			nlohmann::json({{"errorcode", ec}, {"overview", overview}, {"data", data}}));
		if (ec[0] == 'E') success = false;
	}

	nlohmann::json result_json;
	bool success;

	operator bool() { return success; }
	bool operator!() { return !success; }
	
	std::string dump() const;
};

inline Result operator+(const Result& lhs, const Result& rhs)
{
	Result ret;
	ret.success = !(!lhs.success || !rhs.success);  // if either of them are false, then result is

	// copy each of the results in
	std::copy(lhs.result_json.begin(), lhs.result_json.end(), std::back_inserter(ret.result_json));
	std::copy(rhs.result_json.begin(), rhs.result_json.end(), std::back_inserter(ret.result_json));

	return ret;
}

inline Result& operator+=(Result& lhs, const Result& rhs)
{
	lhs.success = !(!lhs.success || !rhs.success);  // if either of them are false, then result is

	// copy each of the results in
	std::copy(rhs.result_json.begin(), rhs.result_json.end(), std::back_inserter(lhs.result_json));

	return lhs;
}

inline std::ostream& operator<<(std::ostream& lhs, const Result& rhs) {
    lhs << rhs.dump();

    return lhs;
}
}

#endif  // CHIG_RESULT_HPP
