/// \file DefaultModuleCache.cpp

#include "chi/DefaultModuleCache.hpp"

#include <llvm-c/BitWriter.h>

#include <cassert>

#include "chi/BitcodeParser.hpp"
#include "chi/Context.hpp"
#include "chi/ModuleCache.hpp"
#include "chi/Support/Result.hpp"

namespace chi {

DefaultModuleCache::DefaultModuleCache(chi::Context& ctx) : ModuleCache{ctx} {}

Result DefaultModuleCache::cacheModule(const std::filesystem::path& moduleName,
                                       LLVMModuleRef compiledModule, time_point timeAtFileRead) {
	assert(!moduleName.empty() &&
	       "Cannot pass a empty module name to DefaultModuleCache::cacheModule");

	Result res;

	auto cachePath = cachePathForModule(moduleName);

	// make the directories
	std::filesystem::create_directories(cachePath.parent_path());

	// open & write
	if (LLVMWriteBitcodeToFile(compiledModule, cachePath.c_str()) != 0) {
		res.addEntry("EUKN", "Failed to open file", {{"Path", cachePath.string()}});
		return res;
	}

	// set age to be correct
	std::filesystem::last_write_time(cachePath, timeAtFileRead);

	return res;
}

std::filesystem::path DefaultModuleCache::cachePathForModule(
    const std::filesystem::path& moduleName) const {
	return context().workspacePath() / "lib" / (moduleName.string() + ".bc");
}

void DefaultModuleCache::invalidateCache(const std::filesystem::path& moduleName) {
	assert(!moduleName.empty() && "Cannot pass empty path to DefaultModuleCache::invalidateCache");

	auto cachePath = cachePathForModule(moduleName);

	std::filesystem::remove(cachePath);
}

ModuleCache::time_point DefaultModuleCache::cacheUpdateTime(
    const std::filesystem::path& moduleName) const {
	return std::filesystem::last_write_time(cachePathForModule(moduleName));
}

OwnedLLVMModule DefaultModuleCache::retrieveFromCache(const std::filesystem::path& moduleName,
                                                      time_point                   atLeastThisNew) {
	assert(!moduleName.empty() &&
	       "Cannot pass empty path to DefaultModuleCache::retrieveFromCache");

	auto cachePath = cachePathForModule(moduleName);

	// if there is no cache, then there is nothing to retrieve
	if (!std::filesystem::is_regular_file(cachePath)) { return nullptr; }

	// see if the cache is new enough
	auto cacheEditTime = cacheUpdateTime(moduleName);
	if (cacheEditTime < atLeastThisNew) { return nullptr; }

	// read the cache
	OwnedLLVMModule fetchedMod;
	auto            res = parseBitcodeFile(cachePath, context().llvmContext(), &fetchedMod);

	if (!res) { return nullptr; }

	return fetchedMod;
}

}  // namespace chi
