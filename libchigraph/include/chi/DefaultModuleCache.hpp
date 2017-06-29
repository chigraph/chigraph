/// \file DefaultModuleCache.h

#ifndef CHI_DEFAULT_MODULE_CACHE_HPP
#define CHI_DEFAULT_MODULE_CACHE_HPP

#include "chi/ModuleCache.hpp"

namespace chi {

struct DefaultModuleCache : public ModuleCache {

	/// Default constrcutor
	/// \param ctx The context to cache for
	DefaultModuleCache(Context& ctx);

	/// Get the cache name for a module. Basically `context().workspacePath() / moduleName + ".bc"`
	/// \param moduleName The name of the module to get a cache path for
	/// \return The path
	boost::filesystem::path cachePathForModule(const boost::filesystem::path& moduleName) const;

	/// \copydoc ModuleCache::cacheModule
	Result cacheModule(const boost::filesystem::path& moduleName, llvm::Module& compiledModule,
	                   std::time_t timeAtFileRead) override;

	/// \copydoc ModuleCache::invalidateModule
	void invalidateCache(const boost::filesystem::path& moduleName) override;

	/// \copydoc ModuleCache::cacheUpdateTime
	std::time_t cacheUpdateTime(const boost::filesystem::path& moduleName) const override;

	/// \copydoc ModuleCache::retrieveFromCache
	std::unique_ptr<llvm::Module> retrieveFromCache(const boost::filesystem::path& moduleName,
	                                                std::time_t                    atLeastThisNew) override;

};

}

#endif // CHI_DEFAULT_MODULE_CACHE_HPP
