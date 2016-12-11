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
struct CModule : ChigModule {
	CModule(Context& ctx);
	~CModule() = default;

	virtual Result nodeTypeFromName(gsl::cstring_span<> typeName, const nlohmann::json& json_data,
		std::unique_ptr<NodeType>* toFill) override;
	virtual DataType typeFromName(gsl::cstring_span<> typeName) override;

	virtual std::vector<std::string> nodeTypeNames() const override { return {"func"}; };
	virtual std::vector<std::string> typeNames() const override { return {}; };
	std::unordered_map<std::string, std::function<std::unique_ptr<NodeType>(const nlohmann::json&)>>
		nodes;

	Result generateModule(std::unique_ptr<llvm::Module>* module) override { return {}; };
};
}

#endif  // CHIG_CMODULE_HPP
