#ifndef CHIG_CHIG_MODULE_HPP
#define CHIG_CHIG_MODULE_HPP

#pragma once

#include "chig/Fwd.hpp"
#include "chig/ToString.hpp"
#include "chig/Result.hpp"
#include "chig/json.hpp"

#include <cstdlib>
#include <functional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <llvm/IR/Module.h>

#include <gsl/gsl>

namespace chig
{
/// An abstract class that represents a module of code in Chigraph
/// Can be compiled to a llvm::Module
struct ChigModule {
	/// Default constructor. This is usually run by Context::addModule
	/// \param contextArg The context to create the module insides
	/// \param fullName The full name of the module
	ChigModule(Context& contextArg, std::string fullName);

	/// Destructor
	virtual ~ChigModule() = default;

	/// Create a node type that is in the module from the name and json
	/// \param name The name of the node type to create
	/// \param json_data The extra JSON data for the node
	/// \param retType The NodeType object to fill
	/// \return The result
	virtual Result nodeTypeFromName(gsl::cstring_span<> name, const nlohmann::json& json_data,
		std::unique_ptr<NodeType>* retType) = 0;

	/// Get a DataType from the name
	/// \param name The name of the type
	/// \return The data type, or an invalid DataType if failed
	virtual DataType typeFromName(gsl::cstring_span<> name) = 0;

	/// Get the possible node type names
	/// \return A std::vector of the possible names
	virtual std::vector<std::string> nodeTypeNames() const = 0;

	/// Get the possible DataType names
	virtual std::vector<std::string> typeNames() const = 0;

	/// Get the short name of the module (the last bit)
	/// \return The name
	std::string name() const { return mName; }
	/// Get the full name of the module
	/// \return The name
	std::string fullName() const { return mFullName; }
	/// Get the Context that this module belongs to
	/// \return The context
	Context& context() const { return *mContext; }
	/// Generate a llvm::Module from the module. Usually called by Context::compileModule
	/// \param module The llvm::Module to fill -- must be already filled with dependencies
	/// \return The result
	virtual Result generateModule(std::unique_ptr<llvm::Module>* module) = 0;

	/// Get the dependencies
	/// \return The dependencies
	const std::unordered_set<std::string>& dependencies() const { return mDependencies; }
	/// Add a dependency to the module
	/// \param newDepFullPath The dependency
	/// \return The result
	Result addDependency(std::string newDepFullPath);

	/// Remove a dependency
	/// \param depName The name of the dependency to remove
	/// \return If one was removed
	bool removeDependency(gsl::cstring_span<> depName)
	{
		return mDependencies.erase(gsl::to_string(depName)) == 1;
	}

private:
	std::string mFullName;
	std::string mName;
	Context* mContext;

	std::unordered_set<std::string> mDependencies;
};
}

#endif  // CHIG_CHIG_MODULE_HPP
