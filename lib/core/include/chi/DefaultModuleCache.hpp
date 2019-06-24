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
	std::filesystem::path cachePathForModule(const std::filesystem::path& moduleName) const;

	/// \copydoc ModuleCache::cacheModule
	Result cacheModule(const std::filesystem::path& moduleName, LLVMModuleRef compiledModule,
	                   time_point timeAtFileRead) override;

	/// \copydoc ModuleCache::invalidateModule
	void invalidateCache(const std::filesystem::path& moduleName) override;

	/// \copydoc ModuleCache::cacheUpdateTime
	time_point cacheUpdateTime(const std::filesystem::path& moduleName) const override;

	/// \copydoc ModuleCache::retrieveFromCache
	OwnedLLVMModule retrieveFromCache(const std::filesystem::path& moduleName,
	                                  time_point                   atLeastThisNew) override;
};
}  // namespace chi

#endif  // CHI_DEFAULT_MODULE_CACHE_HPP
