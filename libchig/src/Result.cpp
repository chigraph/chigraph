#include "chig/Result.hpp"

namespace chig {

/// \internal
std::string prettyPrintJson(const nlohmann::json& j, int indentLevel) {
	std::string indentString(indentLevel * 2, ' ');

	std::string ret;
	if (j.is_array()) {
		ret += indentString + "[\n";

		for (const auto& elem : j) {
			ret += prettyPrintJson(elem, indentLevel + 1);
			ret += ",\n";
		}
		ret += indentString + "]\n";

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

}  // namespace chig
