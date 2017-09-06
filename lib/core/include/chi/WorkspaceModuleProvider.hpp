/// \file WorkspaceModuleProvider.h

#ifndef CHI_WORKSPACE_MODULE_PROVIDER
#define CHI_WORKSPACE_MODULE_PROVIDER

#include "chi/ModuleCache.hpp"

namespace chi {

/// Provides and caches modules in a workspace format
struct WorkspaceModuleProvider : public ModuleCache {
	/// Default constrcutor
	/// \pre `boost::filesystem::is_regular_file(workspacePath / ".chigraphworkspace")`
	WorkspaceModuleProvider(const boost::filesytem::path& workspacePath);

	/// Read a module from disk
	/// Assumes all dependencies are loaded, will error if not
	/// \param moduleName The name of the module to load
	/// \param toFill The GraphModule to fill
	/// \return The Result
	Result loadModule(const boost::filesystem::path& module, std::unique_ptr<GraphModule>* toFill) override;

	/// Save a module to disk
	/// \param modToSave the module to save
	/// \return The Result
	Result saveModule(const GraphModule& modToSave) const override;

	/// List all the modules in the workspace
	/// \return All the modules
	std::vector<boost::filesystem::path> listModules() const override;

	/// Peek at a module's dependencies without loading it
	/// \param module The  module to get the dependencies
	/// \return The list of direct dependencies (not a recursive search)
	std::vector<boost::filesystem::path> peekDepenencies(const boost::filesystem::path& module) const override;

	/// Get the cache name for a module. Basically `context().workspacePath() / moduleName + ".bc"`
	/// \param moduleName The name of the module to get a cache path for
	/// \return The path
	boost::filesystem::path cachePathForModule(const boost::filesystem::path& moduleName) const override;

	/// \copydoc ModuleCache::cacheModule
	Result cacheModule(const boost::filesystem::path& moduleName, llvm::Module& compiledModule,
	                   std::time_t timeAtFileRead) override;

	/// \copydoc ModuleCache::invalidateModule
	void invalidateCache(const boost::filesystem::path& moduleName) override;

	/// \copydoc ModuleCache::cacheUpdateTime
	std::time_t cacheUpdateTime(const boost::filesystem::path& moduleName) const override;

	/// \copydoc ModuleCache::retrieveFromCache
	std::unique_ptr<llvm::Module> retrieveFromCache(const boost::filesystem::path& moduleName,
	                                                std::time_t atLeastThisNew) override;
	

	boost::filesystem::path workspacePath() const { return mWorkspacePath; }

private:
	boost::filesystem::path mWorkspacePath;
};
}

#endif  // CHI_WORKSPACE_MODULE_PROVIDER
