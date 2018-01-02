/// \file chi/ChiModule.hpp
/// Defines the ChiModule class

#ifndef CHI_CHI_MODULE_HPP
#define CHI_CHI_MODULE_HPP

#pragma once

#include "Fwd.hpp"

#include "chi/Support/HashFilesystemPath.hpp"

#include "json.hpp"

#include <boost/filesystem.hpp>
#include <boost/utility/string_view.hpp>

#include <set>

#include <ctime>

/// The namespace where chigraph lives
namespace chi {
/// An abstract class that represents a module of code in Chigraph
/// Can be compiled to a llvm::Module
struct ChiModule {
	/// Default constructor. This is usually run by Context::loadModule
	/// \param contextArg The context to create the module insides
	/// \param moduleFullName The full name of the module
	ChiModule(Context& contextArg, boost::filesystem::path moduleFullName);

	/// Destructor
	virtual ~ChiModule() = default;

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
	/// \return A `std::vector` of all the names of types this module has
	virtual std::vector<std::string> typeNames() const = 0;

	/// Get the short name of the module (the last bit)
	/// \return The name
	std::string shortName() const { return mName; }
	/// Get the full name of the module
	/// \return The full name
	std::string fullName() const { return mFullName.generic_string(); }
	/// Get the full name of the module in a path
	/// \return The full name, as a boost::filesystem::path
	boost::filesystem::path fullNamePath() const { return mFullName; }
	/// Get the Context that this module belongs to
	/// \return The context

	Context& context() const { return *mContext; }

	/// Adds forward declartions for the functions in this module
	/// \param module The module to add forward declartions to
	/// \return The Result
	virtual Result addForwardDeclarations(llvm::Module& module) const = 0;

	/// Generate a llvm::Module from the module. Usually called by Context::compileModule
	/// \param module The llvm::Module to fill into -- must be already filled with forward
	/// declarations of dependencies
	/// \return The Result
	virtual Result generateModule(llvm::Module& module) = 0;

	/// Get the dependencies
	/// \return The dependencies
	const std::set<boost::filesystem::path>& dependencies() const { return mDependencies; }

	/// Add a dependency to the module
	/// Loads the module from context() if it isn't already loaded
	/// \param newDepFullPath The dependency, full path
	/// \return The result
	Result addDependency(boost::filesystem::path newDepFullPath);

	/// Remove a dependency
	/// Does not unload from context
	/// \param depName The name of the dependency to remove
	/// \return If one was removed
	bool removeDependency(std::string depName) {
		return mDependencies.erase(std::move(depName)) == 1;
	}

	/// Get the time that this module was last edited
	/// \return The `std::time_t` at which it was last edited
	std::time_t lastEditTime() const { return mLastEditTime; }

	/// Update the last edit time, signifying that it's been edited
	/// \param newLastEditTime The new time, or current time for default
	void updateLastEditTime(std::time_t newLastEditTime = std::time(nullptr)) {
		mLastEditTime = newLastEditTime;
	}

private:
	boost::filesystem::path mFullName;
	std::string             mName;
	Context*                mContext;

	std::set<boost::filesystem::path> mDependencies;

	std::time_t mLastEditTime = 0;
};
}  // namespace chi

#endif  // CHI_CHI_MODULE_HPP
