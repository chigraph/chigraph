#ifndef CHIG_JSON_MODULE_HPP
#define CHIG_JSON_MODULE_HPP

#pragma once

#include "chig/ChigModule.hpp"
#include "chig/json.hpp"

#include <vector>

namespace chig {

struct JsonModule : ChigModule {
	

	JsonModule(const nlohamn::json& json_data, Context& cont);
	
	std::vector<GraphFunction*> graphFunctions;
	
	
	
};

}

#endif // CHIG_JSON_MODULE_HPP
