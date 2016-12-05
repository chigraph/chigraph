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
/// Module that holds JSON code
struct JsonModule : public ChigModule {
	/// Constructor for a json module
	/// \param fullName Full path for the module
	/// \param json_data The JSON
	/// \param cont The context
	/// \param res The result to fill if there are errors
	JsonModule(std::string fullName, const nlohmann::json& json_data, Context& cont, Result* res);

	// No copy or move -- pointer only
	JsonModule(const JsonModule&) = delete;
	JsonModule(JsonModule&&) = delete;
	JsonModule& operator=(const JsonModule&) = delete;
	JsonModule& operator=(JsonModule&&) = delete;

	// ChigModule interface
	///////////////////////

	/// \copydoc ChigModule::nodeTypeFromName()
	Result nodeTypeFromName(gsl::cstring_span<> name, const nlohmann::json& jsonData,
		std::unique_ptr<NodeType>* toFill) override;

	/// \copydoc ChigModule::typeFromName
	DataType typeFromName(gsl::cstring_span<> /*name*/) override { return {}; }
	/// \copydoc ChigModule::nodeTypeNames
	virtual std::vector<std::string> nodeTypeNames() const override;  // TODO: implement

	/// \copydoc ChigModule::typeNames
	virtual std::vector<std::string> typeNames() const override { return {}; }  // TODO: implement
	/// \copydoc ChigModule::generateModule
	Result generateModule(std::unique_ptr<llvm::Module>* mod) override;

	/////////////////////

	/// Load the graphs (usually called by Context::addModule)
	Result loadGraphs();

	/// Serialize to JSON
	/// \param to_fill The JSON to fill
	/// \return The result
	Result toJSON(nlohmann::json* to_fill) const;

	/// Get a function from the name
	/// \param name The name to get
	/// \return The GraphFunction or nullptr if it doesn't exist
	GraphFunction* graphFuncFromName(gsl::cstring_span<> name) const;

	/// Get the dependencies
	/// \return The dependencies
	const std::vector<std::string>& dependencies() const { return mDependencies; }
	/// Get functions
	/// \return The functions
	const std::vector<std::unique_ptr<GraphFunction>>& functions() const { return mFunctions; }
private:
	std::vector<std::unique_ptr<GraphFunction>> mFunctions;
	std::vector<std::string> mDependencies;
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
