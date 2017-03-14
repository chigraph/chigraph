/// \file chig/ChigModule.hpp
/// Defines the ChigModule class

#ifndef CHIG_CHIG_MODULE_HPP
#define CHIG_CHIG_MODULE_HPP

#pragma once

#include "chig/Fwd.hpp"
#include "chig/HashFilesystemPath.hpp"
#include "chig/ToString.hpp"
#include "chig/json.hpp"

#include <boost/filesystem.hpp>
#include <boost/utility/string_view.hpp>

#include <unordered_set>

#include <llvm/IR/Module.h>

/// The namespace where chigraph lives
namespace chig {
/// An abstract class that represents a module of code in Chigraph
/// Can be compiled to a llvm::Module
struct ChigModule {
	/// Default constructor. This is usually run by Context::loadModule
	/// \param contextArg The context to create the module insides
	/// \param moduleFullName The full name of the module
	ChigModule(Context& contextArg, boost::filesystem::path moduleFullName);

	/// Destructor
	virtual ~ChigModule() = default;

	/// Create a node type that is in the module from the name and json
	/// \param name The name of the node type to create
	/// \param jsonData The extra JSON data for the node
	/// \retval toFill The NodeType object to fill
	/// \pre toFill isn't null (the value the unique_ptr points to be can be null, but not the
	/// pointer to the unique_ptr)
	/// \return The result
	virtual Result nodeTypeFromName(boost::string_view name, const nlohmann::json& jsonData,
	                                std::unique_ptr<NodeType>* toFill) = 0;

	/// Get a DataType from the name
	/// \param name The name of the type
	/// \return The data type, or an invalid DataType if failed
	virtual DataType typeFromName(boost::string_view name) = 0;

	/// Get the possible node type names
	/// \return A std::vector of the possible names
	virtual std::vector<std::string> nodeTypeNames() const = 0;

	/// Get the possible DataType names
	virtual std::vector<std::string> typeNames() const = 0;

	/// Get the short name of the module (the last bit)
	/// \return The name
	std::string shortName() const { return mName; }
	/// Get the full name of the module
	std::string fullName() const { return mFullName.generic_string(); }
	/// Get the full name of the module in a path
	boost::filesystem::path fullNamePath() const { return mFullName; }
	/// Get the Context that this module belongs to
	/// \return The context
	Context& context() const { return *mContext; }
	/// Generate a llvm::Module from the module. Usually called by Context::compileModule
	/// \param module The llvm::Module to fill into -- must be already filled with dependencies
	/// \return The result
	virtual Result generateModule(llvm::Module& module) = 0;

	/// Get the dependencies
	/// \return The dependencies
	const std::unordered_set<boost::filesystem::path>& dependencies() const {
		return mDependencies;
	}
	/// Add a dependency to the module
	/// Loads the module from context() if it isn't already loaded
	/// \param newDepFullPath The dependency, full path
	/// \return The result
	Result addDependency(const boost::filesystem::path& newDepFullPath);

	/// Remove a dependency
	/// Does not unload from context
	/// \param depName The name of the dependency to remove
	/// \return If one was removed
	bool removeDependency(std::string depName) {
		return mDependencies.erase(std::move(depName)) == 1;
	}

private:
	boost::filesystem::path mFullName;
	std::string             mName;
	Context*                mContext;

	std::unordered_set<boost::filesystem::path> mDependencies;
};
}

#endif  // CHIG_CHIG_MODULE_HPP
