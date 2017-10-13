/// \file WorkspaceModuleProvider.cpp

#include "chi/WorkspaceModuleProvider.hpp"
#include "chi/Context.hpp"
#include "chi/GraphModule.hpp"
#include "chi/JsonSerializer.hpp"
#include "chi/LLVMVersion.hpp"
#include "chi/Support/Result.hpp"

#include <cassert>

#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/operations.hpp>

#include <boost/range.hpp>

#if LLVM_VERSION_LESS_EQUAL(3, 9)
#include <llvm/Bitcode/ReaderWriter.h>
#else
#include <llvm/Bitcode/BitcodeWriter.h>
#endif

#include <llvm/IR/Module.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/raw_ostream.h>

namespace fs = boost::filesystem;

namespace chi {

WorkspaceModuleProvider::WorkspaceModuleProvider(const boost::filesystem::path& workspacePath)
    : mWorkspacePath{workspacePath} {}

Result WorkspaceModuleProvider::loadModule(const boost::filesystem::path& module,
                                           nlohmann::json* toFill, std::time_t* toFillTime) {
	assert(!module.empty() && "Cannot load a module with an empty module name");
	assert(toFill != nullptr && "Cannot load a module with an null toFill");
	assert(toFillTime != nullptr && "Cannot load a module with a null toFillTime");

	Result res;
	if (workspacePath().empty()) {
		res.addEntry("E52", "Cannot load module without a workspace path", {});
		return res;
	}

	// find it in the workspace
	fs::path fullPath = workspacePath() / "src" / module;
	fullPath.replace_extension(".chimod");

	if (!fs::is_regular_file(fullPath)) {
		res.addEntry("EUKN", "Failed to find module",
		             {{"Workspace Path", workspacePath().string()},
		              {"Expected Path", fullPath.generic_string()}});
		return res;
	}

	// set the last edit time
	*toFillTime = fs::last_write_time(fullPath);

	// load the JSON
	nlohmann::json readJson = {};
	try {
		fs::ifstream inFile{fullPath};

		inFile >> readJson;
	} catch (std::exception& e) {
		res.addEntry("EUKN", "Failed to parse json", {{"Error", e.what()}});
		return res;
	}

	return readJson;
}

Result WorkspaceModuleProvider::saveModule(const GraphModule& modToSave) const {
	Result res;

	auto modulePath = pathFromModuleName(modToSave.fullName());

	try {
		// create directories that conatain the path
		fs::create_directories(modulePath.parent_path());

	} catch (std::exception& e) {
		res.addEntry("EUKN", "Failed to create directoires in workspace",
		             {{"Module File", modulePath.string()}});
		return res;
	}

	// serialize
	nlohmann::json toFill = graphModuleToJson(modToSave);

	// save
	fs::ofstream ostr(modulePath);
	ostr << toFill.dump(2);

	return res;
}

boost::filesystem::path WorkspaceModuleProvider::pathToCSources(const GraphModule& module) const {
	return pathFromModuleName(module.fullName()).parent_path() / (module.shortName() + ".c");
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

std::vector<boost::filesystem::path> WorkspaceModuleProvider::peekDepenencies(
    const boost::filesystem::path& module) const {
	auto           filename = pathFromModuleName(module);
	nlohmann::json j;
	try {
		fs::ifstream file{filename};
		file >> j;
	} catch (std::exception& e) { return {}; }

	if (j.find("dependencies") != j.end() || !j["dependencies"].is_array()) { return {}; }

	std::vector<fs::path> ret;
	ret.reserve(j["dependencies"].size());
	for (const auto& item : j["dependencies"]) {
		std::string depName = item;
		ret.emplace_back(depName);
	}

	return ret;
}

Result WorkspaceModuleProvider::cacheModule(const boost::filesystem::path& moduleName,
                                            llvm::Module&                  compiledModule,
                                            std::time_t                    timeAtFileRead) {
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
		std::string          errString;
		llvm::raw_fd_ostream fileStream {
			cachePath.string().c_str(),
#if LLVM_VERSION_LESS_EQUAL(3, 5)
			    errString,
#else
			    errCode,
#endif
			    llvm::sys::fs::F_RW
		};

		if (errCode || !errString.empty()) {
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
	return workspacePath() / "lib" / (moduleName.string() + ".bc");
}

void WorkspaceModuleProvider::invalidateCache(const boost::filesystem::path& moduleName) {
	assert(!moduleName.empty() && "Cannot pass empty path to DefaultModuleCache::invalidateCache");

	auto cachePath = cachePathForModule(moduleName);

	boost::filesystem::remove(cachePath);
}

std::time_t WorkspaceModuleProvider::cacheUpdateTime(
    const boost::filesystem::path& moduleName) const {
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

	// read the cache
	std::unique_ptr<llvm::Module> fetchedMod;
	auto res = parseBitcodeFile(cachePath, context().llvmContext(), &fetchedMod);

	if (!res) { return nullptr; }

	return fetchedMod;
}

fs::path WorkspaceModuleProvider::pathFromModuleName(const fs::path& fullName) {
	return workspacePath() / "src" / (fullName + ".chimod");
}

}  // namespace chi
