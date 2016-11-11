#ifndef CHIG_JSON_MODULE_HPP
#define CHIG_JSON_MODULE_HPP

#pragma once

#include "chig/ChigModule.hpp"
#include "chig/GraphFunction.hpp"
#include "chig/Result.hpp"
#include "chig/json.hpp"

#include <vector>

namespace chig
{
struct JsonModule : ChigModule {
	/// Constructor for a json module
	JsonModule(const nlohmann::json& json_data, Context& cont, Result* res);

	JsonModule(const JsonModule&) = delete;
	JsonModule(JsonModule&&) = delete;

	JsonModule& operator=(const JsonModule&) = delete;
	JsonModule& operator=(JsonModule&&) = delete;

	std::vector<std::unique_ptr<GraphFunction>> functions;

	std::string name;
	std::vector<std::string> dependencies;
};
}

#endif  // CHIG_JSON_MODULE_HPP
