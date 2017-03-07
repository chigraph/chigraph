/// \file chig/CModule.hpp
/// Defines the CModule class

#pragma once

#ifndef CHIG_CMODULE_HPP
#define CHIG_CMODULE_HPP

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

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

	Result nodeTypeFromName(boost::string_view typeName, const nlohmann::json& jsonData,
	                        std::unique_ptr<NodeType>* toFill) override;
	DataType typeFromName(boost::string_view name) override;
	std::vector<std::string> nodeTypeNames() const override { return {"func"}; }
	std::vector<std::string> typeNames() const override { return {}; }
	Result                   generateModule(llvm::Module& /*module*/) override { return {}; }

	/// Set extra arguments to pass to clang for generating C code
	/// \param extraArgs The new extra arguments
	void setExtraArguments(std::vector<std::string> extraArgs) {
		mExtraCArgs = std::move(extraArgs);
	}

	/// Get the extra arguments
	/// \return The extra arguments
	const std::vector<std::string>& extraArguments() const { return mExtraCArgs; }

	Result createNodeTypeFromCCode(const std::string& code, const std::string& functionName,
	                               const std::vector<std::string>& clangArgs,
	                               std::unique_ptr<NodeType>*      toFill);

private:
	std::vector<std::string> mExtraCArgs;
};
}

#endif  // CHIG_CMODULE_HPP
