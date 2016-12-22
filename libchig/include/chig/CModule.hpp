#pragma once

#ifndef CHIG_CMODULE_HPP
#define CHIG_CMODULE_HPP

#include <functional>
#include <memory>
#include <unordered_map>

#include "chig/ChigModule.hpp"
#include "chig/DataType.hpp"
#include "chig/Fwd.hpp"
#include "chig/NodeType.hpp"
#include "chig/json.hpp"

namespace chig
{
/// The CModule is the module that supplies functionality to create C bindings.
struct CModule : ChigModule {
	/// Contstructor--construct with a context. This is usually called by Context
	CModule(Context& ctx);

	/// Destructor
	~CModule() = default;

	/// \copydoc chig::ChigModule::nodeTypeFromName
	virtual Result nodeTypeFromName(gsl::cstring_span<> typeName, const nlohmann::json& json_data,
		std::unique_ptr<NodeType>* toFill) override;
	/// \copydoc chig::ChigModule::typeFromName
	virtual DataType typeFromName(gsl::cstring_span<> typeName) override;

	/// \copydoc chig::ChigModule::nodeTypeNames
	virtual std::vector<std::string> nodeTypeNames() const override { return {"func"}; };
	/// \copydoc  chig::ChigModule::typeNames
	virtual std::vector<std::string> typeNames() const override { return {}; };
	/// \copydoc chig::ChigModule::generateModule
	Result generateModule(std::unique_ptr<llvm::Module>* /*module*/) override { return {}; };
};
}

#endif  // CHIG_CMODULE_HPP
