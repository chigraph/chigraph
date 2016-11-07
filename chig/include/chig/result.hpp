#ifndef CHIG_RESULT_HPP
#define CHIG_RESULT_HPP

#pragma once

#include "chig/json.hpp"

namespace chig {

struct Result {
    Result(nlohmann::json json = nlohmann::json::array()) : result_json{std::move(json)} {
        if(!result_json.is_array()) {
            // must be an array!
            assert(false);
        }
    }


    nlohmann::json result_json = nlohmann::json::array();
    bool success;

    operator bool() {
        return success;
    }
    bool operator !()  {
        return !success;
    }

};

inline Result operator+(const Result& lhs, const Result& rhs) {
    Result ret;
    ret.success = !(!lhs.success || !rhs.success); // if either of them are false, then result is

    // copy each of the results in
    std::copy(lhs.result_json.begin(), lhs.result_json.end(), std::back_inserter(ret.result_json));
    std::copy(rhs.result_json.begin(), rhs.result_json.end(), std::back_inserter(ret.result_json));

    return ret;
}

inline Result& operator+=(Result& lhs, const Result& rhs) {
    lhs.success = !(!lhs.success || !rhs.success); // if either of them are false, then result is

    // copy each of the results in
    std::copy(rhs.result_json.begin(), rhs.result_json.end(), std::back_inserter(lhs.result_json));

    return lhs;
}

}


#endif // CHIG_RESULT_HPP
