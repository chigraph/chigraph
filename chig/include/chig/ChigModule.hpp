#ifndef CHIG_CHIG_MODULE_HPP
#define CHIG_CHIG_MODULE_HPP

#pragma once

#include <cstdlib>
#include "chig/ToString.hpp"
#include "chig/json.hpp"

#include <llvm/IR/Type.h>

#include <functional>
#include <string>
#include <vector>

namespace chig
{
struct Context;
struct NodeType;

struct ChigModule {
	ChigModule(Context& contextArg) : context{&contextArg} {}
	virtual ~ChigModule() = default;

	virtual std::unique_ptr<NodeType> createNodeType(
		const char* name, const nlohmann::json& json_data) const = 0;
	virtual llvm::Type* getType(const char* name) const = 0;

	std::string name;

	Context* context;
};
}

#endif  // CHIG_CHIG_MODULE_HPP
