#include "chig/Result.hpp"

namespace chig {

std::string prettyPrintJson(const nlohmann::json& j, int indentLevel) {
    std::string ret;
    if(j.is_array()) {
        ret += std::string(indentLevel, '\t') + "[\n";
        
        for(auto elem : j) {
            ret += prettyPrintJson(elem, indentLevel + 1);
            ret += ",\n";
        }
        ret += std::string(indentLevel, '\t') + "]\n";
    
    } else if(j.is_string() || j.is_number()) {
        ret += std::string(indentLevel, '\t') + j.dump();
    } else if(j.is_object()) {
        for(auto iter = j.begin(); iter != j.end(); ++iter) {
            ret += std::string(indentLevel, '\t') + iter.key() + "\n";
            ret += prettyPrintJson(iter.value(), indentLevel + 1);
        }
    }
    return ret;
}

std::string Result::dump() const  {
    std::string ret;
    if(result_json.size() != 0) {
        for(auto error : result_json) {
            if(error.find("errorcode") == error.end()
                    || !error["errorcode"].is_string()
                    || error.find("overview") == error.end()
                    || !error["overview"].is_string()){
                return "";
            }
            std::string ec = error["errorcode"];
            std::string desc = error["overview"];
            ret += ec + ": " + desc;
            // recursively display children
            auto& data = error["data"];
            ret += prettyPrintJson(data, 1);
        }
    }
    return ret;
}

} // namespace chig
