#ifndef CHIG_LANG_MODULE_HPP
#define CHIG_LANG_MODULE_HPP

#pragma once

#include "chig/ChigModule.hpp"
#include "chig/Context.hpp"
#include "chig/DataType.hpp"
#include "chig/NodeType.hpp"

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

namespace chig
{
/// The module that provides built-in operations like literals, math operations, etc
struct LangModule : ChigModule {
	/// Default constructor, usually called from Context::loadModule("lang")
	LangModule(Context& ctx);

	/// Destructor
	~LangModule() = default;

	/// \copydoc chig::ChigModule::nodeTypeFromName
	virtual Result nodeTypeFromName(gsl::cstring_span<> name, const nlohmann::json& json_data,
		std::unique_ptr<NodeType>* toFill) override;

	/// \copydoc chig::ChigModule::typeFromName
	virtual DataType typeFromName(gsl::cstring_span<> name) override;

	/// \copydoc chig::ChigModule::nodeTypeNames
	virtual std::vector<std::string> nodeTypeNames() const override
	{
		return {"if", "entry", "exit", "const-int", "strliteral", "const-bool"};
	}

	/// \copydoc chig::ChigModule::typeNames
	virtual std::vector<std::string> typeNames() const override
	{
		return {"i32", "i1", "double"};  // TODO: do i need more?
	}

	/// \copydoc chig::ChigModule::generateModule
	Result generateModule(std::unique_ptr<llvm::Module>* /*module*/) override { return {}; };
private:
	std::unordered_map<std::string,
		std::function<std::unique_ptr<NodeType>(const nlohmann::json&, Result&)>>
		nodes;
};
}

#endif  // CHIG_LANG_MODULE_HPP
