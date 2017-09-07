/// \file WorkspaceModuleProvider.h

#ifndef CHI_WORKSPACE_MODULE_PROVIDER
#define CHI_WORKSPACE_MODULE_PROVIDER

#include "chi/ModuleProvider.hpp"

namespace chi {

/// Provides and caches modules in a workspace format
struct WorkspaceModuleProvider : public ModuleProvider {
	/// Default constrcutor
	/// \pre `boost::filesystem::is_regular_file(workspacePath / ".chigraphworkspace")`
	WorkspaceModuleProvider(const boost::filesystem::path& workspacePath);

	/// \copydoc ModuleProvider::loadModule
	Result loadModule(const boost::filesystem::path& module, std::unique_ptr<GraphModule>* toFill) override;

	/// \copydoc ModuleProvider::saveModule
	Result saveModule(const GraphModule& modToSave) const override;

	/// \copydoc ModuleProvider::listModules
	std::vector<boost::filesystem::path> listModules() const override;

	/// \copydoc ModuleProvider::peekDepenencies
	std::vector<boost::filesystem::path> peekDepenencies(const boost::filesystem::path& module) const override;

	boost::filesystem::path cachePathForModule(const boost::filesystem::path& moduleName) const;

	/// \copydoc ModuleProvider::cacheModule
	Result cacheModule(const boost::filesystem::path& moduleName, llvm::Module& compiledModule,
	                   std::time_t timeAtFileRead) override;

	/// \copydoc ModuleProvider::invalidateModule
	void invalidateCache(const boost::filesystem::path& moduleName) override;

	/// \copydoc ModuleProvider::cacheUpdateTime
	std::time_t cacheUpdateTime(const boost::filesystem::path& moduleName) const override;

	/// \copydoc ModuleProvider::retrieveFromCache
	std::unique_ptr<llvm::Module> retrieveFromCache(const boost::filesystem::path& moduleName,
	                                                std::time_t atLeastThisNew) override;
	

	boost::filesystem::path workspacePath() const { return mWorkspacePath; }

private:
	boost::filesystem::path mWorkspacePath;
};
}

#endif  // CHI_WORKSPACE_MODULE_PROVIDER
