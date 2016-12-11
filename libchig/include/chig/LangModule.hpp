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
struct LangModule : ChigModule {
	LangModule(Context& ctx);
	~LangModule() = default;

	virtual Result nodeTypeFromName(gsl::cstring_span<> name, const nlohmann::json& json_data,
		std::unique_ptr<NodeType>* toFill) override;
	virtual DataType typeFromName(gsl::cstring_span<> name) override;

	virtual std::vector<std::string> nodeTypeNames() const override
	{
		return {"if", "entry", "exit", "const-int", "strliteral", "const-bool"};
	}
	virtual std::vector<std::string> typeNames() const override
	{
		return {"i32", "i1", "double"};  // TODO: do i need more?
	}
	Result generateModule(std::unique_ptr<llvm::Module>* module) override
	{
		*module = std::make_unique<llvm::Module>("lang", context().llvmContext());

		return {};
	};

	std::unordered_map<std::string,
		std::function<std::unique_ptr<NodeType>(const nlohmann::json&, Result&)>>
		nodes;
};
}

#endif  // CHIG_LANG_MODULE_HPP
