/// \file chig/CModule.hpp
/// Defines the CModule class

#pragma once

#ifndef CHIG_CMODULE_HPP
#define CHIG_CMODULE_HPP

#include <functional>
#include <memory>
#include <unordered_map>

#include "chig/ChigModule.hpp"
#include "chig/Fwd.hpp"
#include "chig/NodeType.hpp"
#include "chig/json.hpp"

namespace chig {
/// The CModule is the module that supplies functionality to create C bindings.
struct CModule : ChigModule {
	/// Contstructor--construct with a context. This is usually called by Context
	CModule(Context& ctx);

	/// Destructor
	~CModule() = default;

	Result nodeTypeFromName(gsl::cstring_span<> typeName, const nlohmann::json& jsonData,
	                        std::unique_ptr<NodeType>* toFill) override;
	DataType typeFromName(gsl::cstring_span<> name) override;
	llvm::DIType* debugTypeFromName(gsl::cstring_span<> /*name*/) override { return nullptr; }
	std::vector<std::string> nodeTypeNames() const override { return {"func"}; }
	std::vector<std::string> typeNames() const override { return {}; }
	Result                   generateModule(llvm::Module& /*module*/) override { return {}; }
};
}

#endif  // CHIG_CMODULE_HPP
