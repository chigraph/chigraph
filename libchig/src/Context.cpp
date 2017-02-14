/// \file Context.cpp

#include "chig/GraphFunction.hpp"
#include "chig/GraphModule.hpp"
#include "chig/GraphStruct.hpp"
#include "chig/JsonDeserializer.hpp"
#include "chig/LangModule.hpp"
#include "chig/NodeInstance.hpp"

#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Linker/Linker.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Target/TargetOptions.h>

#include <boost/filesystem.hpp>
#include <boost/range.hpp>
#include <boost/algorithm/string/replace.hpp>

#include <llvm/Target/TargetMachine.h>
#include <chig/CModule.hpp>
#include <gsl/gsl>

using namespace llvm;

namespace fs = boost::filesystem;

namespace chig {
Context::Context(const boost::filesystem::path& workPath) {
	mLLVMContext = std::make_unique<llvm::LLVMContext>();

	mWorkspacePath = workspaceFromChildPath(workPath);
}
ChigModule* Context::moduleByName(gsl::cstring_span<> moduleName) const noexcept {
	Result res;

	for (auto& module : mModules) {
		if (module->name() == moduleName) { return module.get(); }
	}
	return nullptr;
}

ChigModule* Context::moduleByFullName(gsl::cstring_span<> fullModuleName) const noexcept {
	Result res;

	for (auto& module : mModules) {
		if (module->fullName() == fullModuleName) { return module.get(); }
	}
	return nullptr;
}

GraphModule* Context::newGraphModule(gsl::cstring_span<> fullName) {
	// create the module
	GraphModule* mod = nullptr;
	{
		auto uMod = std::make_unique<GraphModule>(*this, gsl::to_string(fullName),
		                                          gsl::span<std::string>());

		mod = uMod.get();
		addModule(std::move(uMod));
	}

	return mod;
}

std::unordered_set<std::string> Context::listModulesInWorkspace() const noexcept {
	std::unordered_set<std::string> moduleList;

	fs::path srcDir = workspacePath() / "src";

	if (!fs::is_directory(srcDir)) { return {}; }

	for (const auto& dirEntry : boost::make_iterator_range(
	         fs::recursive_directory_iterator{srcDir, fs::symlink_option::recurse}, {})) {
		const fs::path& p = dirEntry;

		// see if it's a chigraph module
		if (fs::is_regular_file(p) && p.extension() == ".chigmod") {
			fs::path relPath = fs::relative(p, srcDir);

			relPath.replace_extension("");  // remove .chigmod
			moduleList.insert(relPath.string());
		}
	}

	return moduleList;
}

chig::Result chig::Context::loadModule(const gsl::cstring_span<> name, ChigModule** toFill) {
	Result res;

	// check for built-in modules
	if (name == "lang") {
		auto mod = std::make_unique<LangModule>(*this);
		if (toFill != nullptr) { *toFill = mod.get(); }
		addModule(std::move(mod));
		return {};
	}
	if (name == "c") {
		auto mod = std::make_unique<CModule>(*this);
		if (toFill != nullptr) { *toFill = mod.get(); }
		addModule(std::move(mod));  // we don't care if it's actually added
		return {};
	}

	if (workspacePath().empty()) {
		res.addEntry("EUKN", "Cannot load module without a workspace path",
		             {{"Requested Module"}, gsl::to_string(name)});
		return res;
	}

	// find it in the workspace
	fs::path fullPath = workspacePath() / "src" / (gsl::to_string(name) + ".chigmod");

	if (!fs::is_regular_file(fullPath)) {
		res.addEntry(
		    "EUKN", "Failed to find module",
		    {{"Module Name", gsl::to_string(name)}, {"Workspace Path", workspacePath().string()}});
		return res;
	}

	// load the JSON
	nlohmann::json readJson = {};
	try {
		fs::ifstream inFile{fullPath};

		inFile >> readJson;
	} catch (std::exception& e) {
		res.addEntry("EUKN", "Failed to parse json", {{"Error", e.what()}});
		return res;
	}

	auto str = gsl::to_string(name);
	boost::replace_all(str, "\\", "/");

	GraphModule* toFillJson = nullptr;
	res += addModuleFromJson(str, readJson, &toFillJson);
	if (toFill != nullptr) { *toFill = toFillJson; }

	return res;
}

Result Context::addModuleFromJson(gsl::cstring_span<> fullName, const nlohmann::json& json,
                                  GraphModule** toFill) {
	Result res;

	// make sure it's not already added
	{
		auto mod = moduleByFullName(fullName);
		if (mod != nullptr) {
			if (toFill != nullptr) {
				auto casted = dynamic_cast<GraphModule*>(mod);
				if (casted != nullptr) { *toFill = casted; }
			}
			return {};
		}
	}

	// Create the module
	GraphModule* jMod = nullptr;
	res += jsonToGraphModule(*this, json, fullName, &jMod);
	if (toFill != nullptr) { *toFill = jMod; }

	// if we failed, remove the module
	if (!res) { unloadModule(jMod->fullName()); }

	return res;
}

bool Context::addModule(std::unique_ptr<ChigModule> modToAdd) noexcept {
	Expects(modToAdd != nullptr);

	// make sure it's unique
	auto ptr = moduleByFullName(modToAdd->fullName());
	if (ptr != nullptr) { return false; }
	
	if (modToAdd->fullName() == "c") {
		mCModule = dynamic_cast<CModule*>(modToAdd.get());
	} else if (modToAdd->fullName() == "lang") {
		mLangModule = dynamic_cast<LangModule*>(modToAdd.get());
	}

	mModules.push_back(std::move(modToAdd));

	Expects(modToAdd == nullptr);

	return true;
}

bool Context::unloadModule(gsl::cstring_span<> fullName) {
	// find the module, and if we see it then delete it
	for (auto idx = 0ull; idx < mModules.size(); ++idx) {
		if (mModules[idx]->fullName() == fullName) {
			mModules.erase(mModules.begin() + idx);
			return true;
		}
	}

	// if we get here it wasn't removed
	return false;
}

Result Context::typeFromModule(gsl::cstring_span<> module, gsl::cstring_span<> name,
                               DataType* toFill) noexcept {
	Expects(toFill != nullptr);

	Result res;

	ChigModule* mod = moduleByName(module);
	if (mod == nullptr) {
		res.addEntry("E36", "Could not find module", {{"module", gsl::to_string(module)}});
		return res;
	}

	*toFill = mod->typeFromName(name);
	if (!toFill->valid()) {
		res.addEntry("E37", "Could not find type in module",
		             {{"type", gsl::to_string(name)}, {"module", gsl::to_string(module)}});
	}

	return res;
}

Result Context::nodeTypeFromModule(gsl::cstring_span<> moduleName, gsl::cstring_span<> typeName,
                                   const nlohmann::json&      data,
                                   std::unique_ptr<NodeType>* toFill) noexcept {
	Result res;

	auto module = moduleByName(moduleName);
	if (module == nullptr) {
		res.addEntry("E36", "Could not find module", {{"module", gsl::to_string(moduleName)}});
		return res;
	}

	res += module->nodeTypeFromName(typeName, data, toFill);

	return res;
}

Result Context::compileModule(gsl::cstring_span<> fullName, std::unique_ptr<llvm::Module>* toFill) {
	Expects(toFill != nullptr);

	Result res;

	auto chigmod = moduleByFullName(fullName);

	if (chigmod == nullptr) {
		res.addEntry("E36", "Could not find module", {{"module", gsl::to_string(fullName)}});
		return res;
	}

	auto llmod = std::make_unique<llvm::Module>(gsl::to_string(fullName), llvmContext());

	// generate dependencies
	for (const auto& depName : chigmod->dependencies()) {
		std::unique_ptr<llvm::Module> compiledDep;
		res += compileModule(depName, &compiledDep);  // TODO: detect circular dependencies

		if (!res) { return res; }

// link it in
#if LLVM_VERSION_MAJOR <= 3 && LLVM_VERSION_MINOR <= 7
		llvm::Linker::LinkModules(llmod.get(), compiledDep.get());
#else
		llvm::Linker::linkModules(*llmod, std::move(compiledDep));
#endif
	}

	res += chigmod->generateModule(*llmod);

	// set debug info version if it doesn't already have it
	if (llmod->getModuleFlag("Debug Info Version") == nullptr) {
		llmod->addModuleFlag(llvm::Module::Warning, "Debug Info Version",
		                     llvm::DEBUG_METADATA_VERSION);
	}

	// verify the created module
	if (res) {
		bool        errored;
		std::string err;
		{
			llvm::raw_string_ostream os(err);
			errored = llvm::verifyModule(*llmod, &os);
		}

		if (errored) {
			std::string moduleStr;
			{
				llvm::raw_string_ostream printerStr{moduleStr};
				llmod->print(printerStr, nullptr);
			}

			res.addEntry(
			    "EINT", "Internal compiler error: Invalid module created",
			    {{"Error", err}, {"Full Name", gsl::to_string(fullName)}, {"Module", moduleStr}});
		}
	}

	*toFill = std::move(llmod);

	return res;
}

std::string Context::fullModuleName(gsl::cstring_span<> shortName) const {
	auto mod = moduleByName(shortName);

	if (mod != nullptr) { return mod->fullName(); }

	return "";
}
boost::filesystem::path workspaceFromChildPath(const boost::filesystem::path& path) {
	fs::path ret = path;

	// initialize workspace directory
	// go up until it is a workspace
	while (!ret.empty() && !fs::is_regular_file(ret / ".chigraphworkspace")) {
		ret = ret.parent_path();
	}

	return ret;  // it's ok if it's empty
}

std::string stringifyLLVMType(llvm::Type* ty) {
	Expects(ty != nullptr);

	std::string data;
	{
		llvm::raw_string_ostream stream{data};
		ty->print(stream);
	}
	return data;
}

}  // namespace chig
