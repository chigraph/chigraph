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

#include <boost/bimap.hpp>

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
	/// \param cont The context
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

	Result nodeTypeFromName(gsl::cstring_span<> name, const nlohmann::json& jsonData,
		std::unique_ptr<NodeType>* toFill) override;

	DataType typeFromName(gsl::cstring_span<> /*name*/) override { return {}; }
	llvm::DIType* debugTypeFromName(gsl::cstring_span<> /*name*/) override { return nullptr; }
	std::vector<std::string> nodeTypeNames() const override;  // TODO: implement

	std::vector<std::string> typeNames() const override { return {}; }  // TODO: implement
	Result generateModule(llvm::Module& module) override;

	/////////////////////
    
    /// Create the associations from line number and function in debug info
    /// \return A bimap of function to line number
    boost::bimap<unsigned, GraphFunction*> createLineNumberAssoc() const;

	/// Load the graphs (usually called by Context::addModule)
	Result loadGraphs();

	/// Serialize to JSON
	/// \param to_fill The JSON to fill
	/// \return The Result
	Result toJSON(nlohmann::json* to_fill) const;

	/// Serialize to disk in the context
	/// \return The Result
	Result saveToDisk() const;

	/// Get the path to the source file
	/// It's not garunteed to exist, because it could have not been saved
	/// \return The path
	boost::filesystem::path sourceFilePath() const
	{
		return context().workspacePath() / "src" / (fullName() + ".chigmod");
	}

	/// Create a new function if it does't already exist
	/// \param name The name of the new function
	/// \param dataIns The data inputs to the function
	/// \param dataOuts The data outputs to the function
	/// \param execIns The exec inputs to the function
	/// \param execOuts The exec outputs to the function
	/// \param toFill The new GraphFunction, optional
	/// \return True if a new function was created, false otherwise
	bool createFunction(gsl::cstring_span<> name,
		std::vector<std::pair<DataType, std::string>> dataIns,
		std::vector<std::pair<DataType, std::string>> dataOuts, std::vector<std::string> execIns,
		std::vector<std::string> execOuts, GraphFunction** toFill = nullptr);

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
}

#endif  // CHIG_JSON_MODULE_HPP
