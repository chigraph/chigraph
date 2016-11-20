#ifndef CHIG_JSON_MODULE_HPP
#define CHIG_JSON_MODULE_HPP

#pragma once

#include "chig/Fwd.hpp"
#include "chig/ToString.hpp"
#include "chig/json.hpp"

#include "chig/ChigModule.hpp"

#include <vector>

namespace chig
{
struct JsonModule : public ChigModule {
	/// Constructor for a json module
	JsonModule(const nlohmann::json& json_data, Context& cont, Result* res);

	JsonModule(const JsonModule&) = delete;
	JsonModule(JsonModule&&) = delete;

	JsonModule& operator=(const JsonModule&) = delete;
	JsonModule& operator=(JsonModule&&) = delete;
	
	std::unique_ptr<NodeType> createNodeType(
		const char* name, const nlohmann::json& json_data) const override {return nullptr;}
	llvm::Type* getType(const char* name) const override {return nullptr;}

	Result toJSON(nlohmann::json* to_fill) const;
	
	std::vector<std::unique_ptr<GraphFunction>> functions;
	
	Result compile(std::unique_ptr<llvm::Module>* mod) const;

	std::vector<std::string> dependencies;
};
}

#endif  // CHIG_JSON_MODULE_HPP
