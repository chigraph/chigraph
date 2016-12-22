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

#include <iostream>
#include <unordered_set>
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
	JsonModule(Context& cont, std::string fullName, const nlohmann::json& json_data, Result* res);

	/// Construct a JsonModule from scratch, no json
	/// \param const The context
	/// \param fullName The full name of the module
	/// \param dependencies The dependencies
	JsonModule(Context& cont, std::string fullName, gsl::span<std::string> dependencies);

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

	/// Create a new function
	/// \param name The name of the new function
	/// \param ins The inputs to the function
	/// \param outs The outputs to the function
	/// \param toFill The new GraphFunction, optional
	/// \return The Result
	Result createFunction(gsl::cstring_span<> name,
		std::vector<std::pair<DataType, std::string>> ins,
		std::vector<std::pair<DataType, std::string>> outs, GraphFunction** toFill = nullptr);

    /// Remove a function from the module
    /// \param name The name of the function to remove
    /// \return True if there was a function matching name that was removed
    bool removeFunction(gsl::cstring_span<> name);

    /// Remove a function from the module
    /// \param func The function to remove
    void removeFunction(GraphFunction* func);

	/// Get a function from the name
	/// \param name The name to get
	/// \return The GraphFunction or nullptr if it doesn't exist
	GraphFunction* graphFuncFromName(gsl::cstring_span<> name) const;

	/// Get functions
	/// \return The functions
	const std::vector<std::unique_ptr<GraphFunction>>& functions() const { return mFunctions; }
private:
	std::vector<std::unique_ptr<GraphFunction>> mFunctions;
};

struct JsonFuncCallNodeType : public NodeType {
	JsonFuncCallNodeType(JsonModule& json_module, gsl::cstring_span<> funcname, Result* resPtr);

	Result codegen(size_t execInputID, llvm::Module* mod, llvm::Function* f,
		const gsl::span<llvm::Value*> io, llvm::BasicBlock* codegenInto,
		const gsl::span<llvm::BasicBlock*> outputBlocks) const override;

	nlohmann::json toJSON() const override;

	std::unique_ptr<NodeType> clone() const override;

	JsonModule* JModule;
};
}

#endif  // CHIG_JSON_MODULE_HPP
