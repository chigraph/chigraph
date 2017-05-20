#include <chi/Context.hpp>
#include <chi/LLVMVersion.hpp>
#include <chi/ModuleCache.hpp>
#include <chi/Result.hpp>

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

ModuleCache::ModuleCache(chi::Context& ctx) : mContext{&ctx} {}

Result ModuleCache::cacheModule(const boost::filesystem::path& moduleName,
                                llvm::Module& compiledModule, std::time_t timeAtFileRead) {
	assert(!moduleName.empty() && "Cannot pass a empty module name to ModuleCache::cacheModule");

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

boost::filesystem::path ModuleCache::cachePathForModule(
    const boost::filesystem::path& moduleName) const {
	return context().workspacePath() / "lib" / (moduleName.string() + ".bc");
}

void ModuleCache::invalidateCache(const boost::filesystem::path& moduleName) {
	assert(!moduleName.empty() && "Cannot pass empty path to ModuleCache::invalidateCache");

	auto cachePath = cachePathForModule(moduleName);

	boost::filesystem::remove(cachePath);
}

std::time_t ModuleCache::cacheUpdateTime(const boost::filesystem::path& moduleName) const {
	return boost::filesystem::last_write_time(cachePathForModule(moduleName));
}

std::unique_ptr<llvm::Module> ModuleCache::retrieveFromCache(
    const boost::filesystem::path& moduleName, std::time_t atLeastThisNew) {
	assert(!moduleName.empty() && "Cannot pass empty path to ModuleCache::retrieveFromCache");

	auto cachePath = cachePathForModule(moduleName);

	// if there is no cache, then there is nothing to retrieve
	if (!boost::filesystem::is_regular_file(cachePath)) { return nullptr; }

	// see if the cache is new enough
	auto cacheEditTime = cacheUpdateTime(moduleName);
	if (cacheEditTime < atLeastThisNew) { return nullptr; }

	// if all of this is true, then we can read the cache
	auto bcFileBufferOrError = llvm::MemoryBuffer::getFile(cachePath.string());
	if (!bcFileBufferOrError) { return nullptr; }

	auto errorOrMod = llvm::parseBitcodeFile(llvm::MemoryBufferRef(*bcFileBufferOrError.get()),
	                                         context().llvmContext());

	if (!errorOrMod) {
#if LLVM_VERSION_AT_LEAST(4, 0)
		auto E = errorOrMod.takeError();

		llvm::handleAllErrors(std::move(E), [](llvm::ErrorInfoBase& err) {});
#endif

		return nullptr;
	}

	return std::unique_ptr<llvm::Module>{std::move(errorOrMod.get())};
}

}  // namespace chi
