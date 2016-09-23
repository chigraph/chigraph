#ifndef CHIG_JSON_MODULE_HPP
#define CHIG_JSON_MODULE_HPP

#pragma once

#include "chig/ChigModule.hpp"
#include "chig/json.hpp"

namespace chig {

struct JsonModule {
	

	JsonModule(const nlohamn::json& json_data);
	
	std::vector<GraphFunction*> graphFunctions;
	
	
	
};

}

#endif // CHIG_JSON_MODULE_HPP
