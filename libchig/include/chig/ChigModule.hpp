#ifndef CHIG_CHIG_MODULE_HPP
#define CHIG_CHIG_MODULE_HPP

#pragma once

#include "chig/Fwd.hpp"
#include "chig/Result.hpp"
#include "chig/ToString.hpp"
#include "chig/json.hpp"

#include <llvm/IR/Type.h>

#include <cstdlib>
#include <functional>
#include <string>
#include <vector>

#include <gsl/gsl>

namespace chig
{
struct ChigModule {
	ChigModule(Context& contextArg);
	virtual ~ChigModule() = default;

	virtual Result createNodeType(gsl::cstring_span<> name, const nlohmann::json& json_data,
		std::unique_ptr<NodeType>* retType) const = 0;
	virtual llvm::Type* getType(gsl::cstring_span<> name) const = 0;

	virtual std::vector<std::string> getNodeTypeNames() const = 0;
	virtual std::vector<std::string> getTypeNames() const = 0;
    
    virtual Result generateModule(std::unique_ptr<llvm::Module>* module) const = 0;

	std::string name;

	Context* context;

	llvm::Module* llmodule = nullptr;
};
}

#endif  // CHIG_CHIG_MODULE_HPP
