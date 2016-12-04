#ifndef CHIG_JSON_MODULE_HPP
#define CHIG_JSON_MODULE_HPP

#pragma once

#include "chig/DataType.hpp"
#include "chig/Fwd.hpp"
#include "chig/ToString.hpp"
#include "chig/json.hpp"

#include "chig/ChigModule.hpp"
#include "chig/NodeType.hpp"

#include <gsl/gsl>

#include <vector>

namespace chig
{
struct JsonModule : public ChigModule {
	/// Constructor for a json module
	JsonModule(const nlohmann::json& json_data, Context& cont, Result* res);

	JsonModule(const JsonModule&) = delete;
	JsonModule(JsonModule&&) = delete;

	JsonModule& operator=(const JsonModule&) = delete;
	JsonModule& operator=(JsonModule&&) = delete;

	Result nodeTypeFromName(gsl::cstring_span<> name, const nlohmann::json& jsonData,
		std::unique_ptr<NodeType>* toFill) override;
	DataType typeFromName(gsl::cstring_span<> /*name*/) override { return {}; }
	virtual std::vector<std::string> 
  nodeTypeNames() const override;  // TODO: implement
	virtual std::vector<std::string> typeNames() const override
	{
		return {};
	}  // TODO: implement

	Result loadGraphs();

	Result toJSON(nlohmann::json* to_fill) const;

	std::vector<std::unique_ptr<GraphFunction>> functions;

	Result generateModule(std::unique_ptr<llvm::Module>* mod) override;

	std::vector<std::string> dependencies;

	GraphFunction* graphFuncFromName(gsl::cstring_span<> name) const;
};

struct JsonFuncCallNodeType : public NodeType {
	JsonFuncCallNodeType(JsonModule& json_module, gsl::cstring_span<> funcname, Result* resPtr);

	Result codegen(size_t execInputID, llvm::Module* mod, llvm::Function* f,
		const gsl::span<llvm::Value*> io, llvm::BasicBlock* codegenInto,
		const gsl::span<llvm::BasicBlock*> outputBlocks) const override;

	nlohmann::json toJSON() const override;

	std::unique_ptr<NodeType> clone() const override;

	const JsonModule* JModule;
};
}

#endif  // CHIG_JSON_MODULE_HPP
