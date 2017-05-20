/// \file Result.cpp

#include "chi/Result.hpp"

namespace {

/// merges `from` into `into`. If an entry is in both, it keeps into.
void mergeJsonIntoConservative(nlohmann::json& into, const nlohmann::json& from) {
	for (const auto& j : nlohmann::json::iterator_wrapper(from)) {
		if (into.find(j.key()) == into.end()) { into[j.key()] = j.value(); }
	}
}

std::string prettyPrintJson(const nlohmann::json& j, int indentLevel) {
	std::string indentString(indentLevel * 2, ' ');

	std::string ret;
	if (j.is_array()) {
		ret += indentString + "[\n";

		for (const auto& elem : j) {
			ret += prettyPrintJson(elem, indentLevel + 1);
			ret += ",\n";
		}
		ret += indentString + "]";

	} else if (j.is_string()) {
		std::string str = j;

		// replace find and indent them
		for (auto idx = str.find('\n'); idx < str.length(); idx = str.find('\n', idx + 1)) {
			str.insert(idx + 1, indentString);  // + 1 because it inserts before
		}
		ret += indentString + str;
	} else if (j.is_number()) {
		ret += indentString + j.dump();
	} else if (j.is_object()) {
		for (auto iter = j.begin(); iter != j.end(); ++iter) {
			ret += indentString + iter.key() + "\n";
			ret += prettyPrintJson(iter.value(), indentLevel + 1) + "\n";
		}
	}
	return ret;
}

}  // anonymous namespace

namespace chi {

void Result::addEntry(const char* ec, const char* overview, nlohmann::json data) {
	assert(ec[0] == 'E' || ec[0] == 'I' || ec[0] == 'W');

	mergeJsonIntoConservative(data, contextJson());

	result_json.push_back(
	    nlohmann::json({{"errorcode", ec}, {"overview", overview}, {"data", data}}));
	if (ec[0] == 'E') mSuccess = false;
}

std::string Result::dump() const {
	std::string ret;
	if (result_json.size() != 0) {
		for (auto error : result_json) {
			if (error.find("errorcode") == error.end() || !error["errorcode"].is_string() ||
			    error.find("overview") == error.end() || !error["overview"].is_string()) {
				return "";
			}
			std::string ec   = error["errorcode"];
			std::string desc = error["overview"];
			ret += ec + ": " + desc + "\n";
			// recursively display children
			auto& data = error["data"];
			ret += prettyPrintJson(data, 1);
		}
	}
	return ret;
}

int Result::addContext(const nlohmann::json& data) {
	static int ctxId = 0;

	mContexts.emplace(ctxId, data);
	return ctxId++;
}

void chi::Result::removeContext(int id) { mContexts.erase(id); }

nlohmann::json Result::contextJson() const {
	// merge all the contexts
	auto merged = nlohmann::json::object();

	for (const auto& ctx : mContexts) { mergeJsonIntoConservative(merged, ctx.second); }

	return merged;
}

Result operator+(const Result& lhs, const Result& rhs) {
	Result ret;
	ret.mSuccess = lhs.success() && rhs.success();  // if either of them are false, then result is

	// copy each of the results in
	std::transform(lhs.result_json.begin(), lhs.result_json.end(),
	               std::back_inserter(ret.result_json), [&](nlohmann::json j) {
		               // apply the context
		               mergeJsonIntoConservative(j["data"], rhs.contextJson());
		               return j;
		           });
	std::transform(rhs.result_json.begin(), rhs.result_json.end(),
	               std::back_inserter(ret.result_json), [&](nlohmann::json j) {
		               // apply context
		               mergeJsonIntoConservative(j["data"], lhs.contextJson());
		               return j;
		           });

	return ret;
}

Result& operator+=(Result& lhs, const Result& rhs) {
	lhs.mSuccess = lhs.success() && rhs.success();  // if either of them are false, then result is

	// change the existing entires in lhs to have rhs's context
	for (auto& entry : lhs.result_json) {
		mergeJsonIntoConservative(entry["data"], rhs.contextJson());
	}

	// copy each of the results in and fix context
	std::transform(rhs.result_json.begin(), rhs.result_json.end(),
	               std::back_inserter(lhs.result_json), [&](nlohmann::json j) {
		               mergeJsonIntoConservative(j["data"], lhs.contextJson());
		               return j;
		           });

	return lhs;
}

}  // namespace chi
