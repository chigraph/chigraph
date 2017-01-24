/// \file chig/LangModule.hpp
/// Defines the LangModule class


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

namespace chig {
/// The module that provides built-in operations like literals, math operations, etc
struct LangModule : ChigModule {
	/// Default constructor, usually called from Context::loadModule("lang")
	LangModule(Context& ctx);

	/// Destructor
	~LangModule() = default;

	Result nodeTypeFromName(gsl::cstring_span<> name, const nlohmann::json& jsonData,
	                        std::unique_ptr<NodeType>* toFill) override;

	llvm::DIType* debugTypeFromName(gsl::cstring_span<> name) override;

	DataType typeFromName(gsl::cstring_span<> name) override;

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
}

#endif  // CHIG_LANG_MODULE_HPP
