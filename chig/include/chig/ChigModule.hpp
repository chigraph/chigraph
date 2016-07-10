#ifndef CHIG_CHIG_MODULE_HPP
#define CHIG_CHIG_MODULE_HPP

#pragma once

#include "chig/NodeType.hpp"
#include "chig/json.hpp"

#include <vector>
#include <functional>
#include <string>

namespace chig {

struct Context;

struct ChigModule {
	
	ChigModule(Context& contextArg) : context{&contextArg}
		{}
	
	virtual ~ChigModule() = default;
	
	virtual std::unique_ptr<NodeType> createNodeType(const char* name, const nlohmann::json& json_data) const = 0;
	virtual llvm::Type* getType(const char* name) = 0;
	
	std::string name;
	
	Context* context;
	
};

}

#endif // CHIG_CHIG_MODULE_HPP
