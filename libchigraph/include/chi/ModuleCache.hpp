#pragma once

#ifndef CHI_MODULE_CACHE_HPP
#define CHI_MODULE_CACHE_HPP

#include <chi/Fwd.hpp>

#include <boost/filesystem/path.hpp>
#include <ctime>

namespace chi {

/// This class caches modules into the lib/ direcotry of the workspace
struct ModuleCache {
	/// Create a module cache with a bound context
	explicit ModuleCache(Context& ctx);

	// No copy or move
	ModuleCache(const ModuleCache&) = delete;
	ModuleCache(ModuleCache&&)      = delete;
	ModuleCache& operator=(const ModuleCache&) = delete;
	ModuleCache& operator=(ModuleCache&&) = delete;

	/// Cache a module
	/// \param moduleName The name of the module to cache
	/// \expects `!moduleName.empty()`
	/// \param compiledModule The IR that's been compiled from this module
	/// \param timeAtFileRead The time to store as the cache time. Should be the time the module was
	/// read from disk
	Result cacheModule(const boost::filesystem::path& moduleName, llvm::Module& compiledModule,
	                   std::time_t timeAtFileRead);

	/// Get the cache name for a module. Basically `context().workspacePath() / moduleName + ".bc"`
	/// \param moduleName The name of the module to get a cache path for
	/// \return The path
	boost::filesystem::path cachePathForModule(const boost::filesystem::path& moduleName) const;

	/// Inavlidate the cache, as it delete the cache file
	/// \param moduleName The name of the module to invalidate
	/// \expects `!moduleName.empty()`
	void invalidateCache(const boost::filesystem::path& moduleName);

	/// Get the time that a cache was updated
	/// \param moduleName The module to get the last update time for
	/// \return The time it was updated, or 0 if there is no cache
	std::time_t cacheUpdateTime(const boost::filesystem::path& moduleName) const;

	/// Retrieve a module from the cache
	/// \param moduleName The name of the module to retrieve
	/// \expects `!moduleName.empty()`
	/// \param atLeastThisNew Make sure the cache is at least as new as this
	/// \return A llvm::Module, or nullptr if no suitable cache was found
	std::unique_ptr<llvm::Module> retrieveFromCache(const boost::filesystem::path& moduleName,
	                                                std::time_t                    atLeastThisNew);

	/// Get the context this cache is bound to
	/// \return the `Context`
	Context& context() const { return *mContext; }

private:
	Context* mContext;
};

}  // namespace chi

#endif  // CHI_MODULE_CACHE_HPP
