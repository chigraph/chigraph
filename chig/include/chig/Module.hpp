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
	
	virtual std::vector<std::function<std::unique_ptr<NodeType>(nlohmann::json& data)>> getNodeTypes() = 0;
	virtual std::unique_ptr<NodeType> createNodeType(const char* name, const nlohmann::json& json_data) = 0;
	
	std::string name;
	
	
};

}

#endif // CHIG_MODULE_HPP
