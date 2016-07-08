#ifndef CHIG_MODULE_HPP
#define CHIG_MODULE_HPP

#pragma once

#include "chig/NodeType.hpp"
#include "chig/json.hpp"

#include <vector>
#include <functional>
#include <string>

namespace chig {

struct Module {
	
	virtual ~Module();
	
	virtual std::vector<std::function<NodeType(nlohmann::json& data)>> getNodeTypes() = 0;
	
	std::string name;
	
	
};

}

#endif // CHIG_MODULE_HPP
