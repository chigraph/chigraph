/// \file ModuleCache.hpp

#pragma once

#ifndef CHI_MODULE_CACHE_HPP
#define CHI_MODULE_CACHE_HPP

#include <chi/Fwd.hpp>
#include <chi/Owned.hpp>
#include <ctime>
#include <filesystem>

namespace chi {

/// This class  provides an interface for creating module caches
struct ModuleCache {
	/// Create a module cache with a bound context
	explicit ModuleCache(Context& ctx) : mContext{&ctx} {}

	/// Destructor
	virtual ~ModuleCache() = default;

	// No copy or move
	ModuleCache(const ModuleCache&) = delete;
	ModuleCache(ModuleCache&&)      = delete;
	ModuleCache& operator=(const ModuleCache&) = delete;
	ModuleCache& operator=(ModuleCache&&) = delete;

	using time_point = std::filesystem::file_time_type;

	/// Cache a module
	/// \param moduleName The name of the module to cache
	/// \pre `!moduleName.empty()`
	/// \param compiledModule The IR that's been compiled from this module
	/// \param timeAtFileRead The time to store as the cache time. Should be the time the module was
	/// read from disk
	/// \return The Result
	virtual Result cacheModule(const std::filesystem::path& moduleName,
	                           LLVMModuleRef compiledModule, time_point timeAtFileRead) = 0;

	/// Inavlidate the cache, ie. delete the cache file
	/// \param moduleName The name of the module to invalidate
	/// \pre `!moduleName.empty()`
	virtual void invalidateCache(const std::filesystem::path& moduleName) = 0;

	/// Get the time that a cache was updated
	/// \param moduleName The module to get the last update time for
	/// \return The time it was updated, or 0 if there is no cache
	virtual time_point cacheUpdateTime(const std::filesystem::path& moduleName) const = 0;

	/// Retrieve a module from the cache
	/// \param moduleName The name of the module to retrieve
	/// \pre `!moduleName.empty()`
	/// \param atLeastThisNew Make sure the cache is at least as new as this
	/// \return A llvm::Module, or nullptr if no suitable cache was found
	virtual OwnedLLVMModule retrieveFromCache(const std::filesystem::path& moduleName,
	                                          time_point                   atLeastThisNew) = 0;

	/// Get the context this cache is bound to
	/// \return the `Context`
	Context& context() const { return *mContext; }

private:
	Context* mContext;
};

}  // namespace chi

#endif  // CHI_MODULE_CACHE_HPP
