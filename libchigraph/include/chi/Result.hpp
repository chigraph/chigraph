/// \file chi/Result.hpp
/// Defines the Result class and related functions

#ifndef CHI_RESULT_HPP
#define CHI_RESULT_HPP

#pragma once

#include "chi/ToString.hpp"
#include "chi/json.hpp"

#include <gsl/gsl>

namespace chi {
/// The result object, used for identifiying errors with good diagnostics
///
/// ## Usage:
/// If you want to construct a default result object, just call the default constructor:
/// ```
/// Result res;
/// ```
/// `res` is the most common name for an error object, prefer it to other names.
///
/// When an error, warning, or just some event that requries logging needs to be added to the result
/// object,
/// call `Result::addEntry`:
/// ```
/// Result res;
/// res.addEntry("E231", "Some error occured", {{"Line Number", 34}});
/// ```
/// Note the `E` at the beginning of the error code, this is important. If it's an `E`, it's
/// considered an error,
/// and will mark the result as errored. If it's a `W`, then it's logged, but _it's still considred
/// successful_.
///
/// If you're calling another operation that emits a Result object, then there's an easy to
/// integrate that result object: the opeartor+=
///
/// ```
/// Result res;
/// res += connectExec(...); // or any function that returns a Result
/// ```
/// ## Standardized names in data:
/// In order to have good parsing of errors, some standards are good.
/// If you are trying to represent this data in your error, use these
/// standards so it can be parsed later on and presented to the user in a nice way
///
/// Name in JSON  | Description
/// ------------- | -------------
/// `Node ID`     | The ID of the node that errored
/// 
/// # What is EUKN?
/// Not every error in chigraph is documented and tested. Errors with numbers are tested and possibly documented.
/// EUKN errors are just undocumented errors.
///
/// ## Implementation Details:
/// Result objects store a json object that represents the error metadata.
/// It's an array, each being an object containing three objects:
/// - `errorcode`: The errorcode. This is an identifier representng the error: a
///    character followed by a number. The value of the character changes the behaviour:
///      - `E`: it's an error and sets `success` to false.
///      - `W`: it's a warning
///      - `I`: it's just info
///
///   If it's anything else, that it will cause an assertion
/// - `overview`: A simple string representing the generics of the problem. It *shouldn't* differ
/// from
///   error to error, if you need to include specifics use the `data` element
/// - `data`: Extra metadata for the error, including context and how the error occured.
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
	operator bool() const { return success; }

	/// !Success test
	bool operator!() const { return !success; }
	/// Dump to a pretty-printed error message
	std::string dump() const;
};

/// \name Result operators
/// \{

/// Append two Result objects
/// \param lhs The left error
/// \param rhs The right error
/// \return The concatinated errors
/// \relates Result
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
/// \relates Result
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
/// \relates Result
inline std::ostream& operator<<(std::ostream& lhs, const Result& rhs) {
	lhs << rhs.dump();

	return lhs;
}

/// \}

}  // namespace chi

#endif  // CHI_RESULT_HPP
