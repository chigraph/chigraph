/// \file chi/LangModule.hpp
/// Defines the LangModule class

#ifndef CHI_LANG_MODULE_HPP
#define CHI_LANG_MODULE_HPP

#pragma once

#include "chi/ChiModule.hpp"
#include "chi/Result.hpp"

#include <unordered_map>

namespace chi {
/// The module that provides built-in operations like literals, math operations, etc
struct LangModule : ChiModule {
	/// Default constructor, usually called from Context::loadModule("lang")
	LangModule(Context& ctx);

	/// Destructor
	~LangModule() = default;

	Result nodeTypeFromName(boost::string_view name, const nlohmann::json& jsonData,
	                        std::unique_ptr<NodeType>* toFill) override;

	DataType typeFromName(boost::string_view name) override;

	std::vector<std::string> nodeTypeNames() const override {
		std::vector<std::string> ret;
		ret.reserve(nodes.size());

		std::transform(nodes.begin(), nodes.end(), std::back_inserter(ret),
		               [](auto pair) { return pair.first; });

		return ret;
	}

	std::vector<std::string> typeNames() const override {
		return {"i32", "i1", "float", "i8*"};  // TODO: do i need more?
	}

	Result generateModule(llvm::Module& /*module*/) override { return {}; };

private:
	std::unordered_map<std::string,
	                   std::function<std::unique_ptr<NodeType>(const nlohmann::json&, Result&)>>
	    nodes;
	std::unordered_map<std::string, llvm::DIType*> mDebugTypes;
};
} // namespace chi

#endif  // CHI_LANG_MODULE_HPP
