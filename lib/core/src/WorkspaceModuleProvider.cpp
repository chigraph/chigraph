/// \file WorkspaceModuleProvider.cpp

#include "chi/WorkspaceModuleProvider.hpp"
#include "chi/Context.hpp"
#include "chi/LLVMVersion.hpp"
#include "chi/ModuleCache.hpp"
#include "chi/Support/Result.hpp"

#include <cassert>

#include <boost/filesystem/operations.hpp>

#if LLVM_VERSION_LESS_EQUAL(3, 9)
#include <llvm/Bitcode/ReaderWriter.h>
#else
#include <llvm/Bitcode/BitcodeReader.h>
#include <llvm/Bitcode/BitcodeWriter.h>
#endif

#include <llvm/Support/FileSystem.h>
#include <llvm/Support/raw_ostream.h>

namespace chi {

WorkspaceModuleProvider::WorkspaceModuleProvider(const boost::filesystem::path& workspacePath) : mWorkspacePath{workspacePath} {}

Result WorkspaceModuleProvider::loadModule(const boost::filesystem::path& module, std::unique_ptr<GraphModule>* toFill) {

}

Result WorkspaceModuleProvider::saveModule(const GraphModule& modToSave) const {
		
} 

std::vector<boost::filesystem::path> WorkspaceModuleProvider::listModules() const {
	std::vector<boost::filesystem::path> moduleList;

	fs::path srcDir = workspacePath() / "src";

	if (!fs::is_directory(srcDir)) { return {}; }

	for (const auto& dirEntry : boost::make_iterator_range(
	         fs::recursive_directory_iterator{srcDir, fs::symlink_option::recurse}, {})) {
		const fs::path& p = dirEntry;

		// see if it's a chigraph module
		if (fs::is_regular_file(p) && p.extension() == ".chimod") {
			fs::path relPath = fs::relative(p, srcDir);

			relPath.replace_extension("");  // remove .chimod
			moduleList.emplace_back(relPath);
		}
	}

	return moduleList;

}

std::vector<boost::filesystem::path> WorkspaceModuleProvider::peekDepenencies(const boost::filesystem::path& module) const {

}

Result WorkspaceModuleProvider::cacheModule(const boost::filesystem::path& moduleName,
                                       llvm::Module& compiledModule, std::time_t timeAtFileRead) {
	assert(!moduleName.empty() &&
	       "Cannot pass a empty module name to DefaultModuleCache::cacheModule");

	Result res;

	auto cachePath = cachePathForModule(moduleName);

	// make the directories
	boost::filesystem::create_directories(cachePath.parent_path());

	// open & write
	{
		// open the file
		std::error_code      errCode;
		llvm::raw_fd_ostream fileStream(cachePath.string(), errCode, llvm::sys::fs::F_RW);

		if (errCode) {
			res.addEntry("EUKN", "Failed to open file", {{"Path", cachePath.string()}});
			return res;
		}

		// write it
		llvm::WriteBitcodeToFile(&compiledModule, fileStream);
	}

	// set age to be correct
	boost::filesystem::last_write_time(cachePath, timeAtFileRead);

	return res;
}

boost::filesystem::path WorkspaceModuleProvider::cachePathForModule(
    const boost::filesystem::path& moduleName) const {
	return context().workspacePath() / "lib" / (moduleName.string() + ".bc");
}

void WorkspaceModuleProvider::invalidateCache(const boost::filesystem::path& moduleName) {
	assert(!moduleName.empty() && "Cannot pass empty path to DefaultModuleCache::invalidateCache");

	auto cachePath = cachePathForModule(moduleName);

	boost::filesystem::remove(cachePath);
}

std::time_t WorkspaceModuleProvider::cacheUpdateTime(const boost::filesystem::path& moduleName) const {
	return boost::filesystem::last_write_time(cachePathForModule(moduleName));
}

std::unique_ptr<llvm::Module> WorkspaceModuleProvider::retrieveFromCache(
    const boost::filesystem::path& moduleName, std::time_t atLeastThisNew) {
	assert(!moduleName.empty() &&
	       "Cannot pass empty path to DefaultModuleCache::retrieveFromCache");

	auto cachePath = cachePathForModule(moduleName);

	// if there is no cache, then there is nothing to retrieve
	if (!boost::filesystem::is_regular_file(cachePath)) { return nullptr; }

	// see if the cache is new enough
	auto cacheEditTime = cacheUpdateTime(moduleName);
	if (cacheEditTime < atLeastThisNew) { return nullptr; }

	// if all of this is true, then we can read the cache
	auto bcFileBufferOrError = llvm::MemoryBuffer::getFile(cachePath.string());
	if (!bcFileBufferOrError) { return nullptr; }

	auto errorOrMod = llvm::parseBitcodeFile(bcFileBufferOrError.get()->getMemBufferRef(),
	                                         context().llvmContext());

	if (!errorOrMod) {
#if LLVM_VERSION_AT_LEAST(4, 0)
		auto E = errorOrMod.takeError();

		llvm::handleAllErrors(std::move(E), [](llvm::ErrorInfoBase& /*err*/) {});
#endif

		return nullptr;
	}

	return std::unique_ptr<llvm::Module>{std::move(errorOrMod.get())};
}

}  // namespace chi
