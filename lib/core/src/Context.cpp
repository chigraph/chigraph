/// \file Context.cpp

#include "chi/Context.hpp"
#include "chi/DefaultModuleCache.hpp"
#include "chi/GraphFunction.hpp"
#include "chi/GraphModule.hpp"
#include "chi/GraphStruct.hpp"
#include "chi/JsonDeserializer.hpp"
#include "chi/LLVMVersion.hpp"
#include "chi/LangModule.hpp"
#include "chi/NodeInstance.hpp"
#include "chi/Support/Result.hpp"

#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/ExecutionEngine/SectionMemoryManager.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Linker/Linker.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>

#include <boost/algorithm/string/replace.hpp>
#include <boost/filesystem.hpp>
#include <boost/range.hpp>

#include <deque>
#include <unordered_set>


namespace fs = boost::filesystem;

namespace chi {
Context::Context(const boost::filesystem::path& workPath) {
	mWorkspacePath = workspaceFromChildPath(workPath);

	mModuleCache = std::make_unique<DefaultModuleCache>(*this);

}

Context::~Context() = default;

ChiModule* Context::moduleByFullName(const boost::filesystem::path& fullModuleName) const noexcept {
	for (auto& module : mModules) {
		if (module->fullName() == fullModuleName) { return module.get(); }
	}
	return nullptr;
}

GraphModule* Context::newGraphModule(const boost::filesystem::path& fullName) {
	// create the module
	GraphModule* mod = nullptr;
	{
		auto uMod = std::make_unique<GraphModule>(*this, fullName);

		mod = uMod.get();
		addModule(std::move(uMod));
	}

	return mod;
}

std::vector<std::string> Context::listModulesInWorkspace() const noexcept {
	std::vector<std::string> moduleList;

	fs::path srcDir = workspacePath() / "src";

	if (!fs::is_directory(srcDir)) { return {}; }

	for (const auto& dirEntry : boost::make_iterator_range(
	         fs::recursive_directory_iterator{srcDir, fs::symlink_option::recurse}, {})) {
		const fs::path& p = dirEntry;

		// see if it's a chigraph module
		if (fs::is_regular_file(p) && p.extension() == ".chimod") {
			fs::path relPath = fs::relative(p, srcDir);

			relPath.replace_extension("");  // remove .chimod
			moduleList.emplace_back(relPath.string());
		}
	}

	return moduleList;
}

Result Context::loadModule(const fs::path& name, ChiModule** toFill) {
	assert(!name.empty() && "Name should not be empty when calling chi::Context::loadModule");

	Result res;

	auto requestedModCtx = res.addScopedContext({{"Requested Module Name", name.generic_string()}});

	// check for built-in modules
	if (name == "lang") {
		if (langModule() != nullptr) {
			if (toFill != nullptr) { *toFill = langModule(); }
			return {};
		}
		auto mod = std::make_unique<LangModule>(*this);
		if (toFill != nullptr) { *toFill = mod.get(); }
		addModule(std::move(mod));
		return {};
	}

	// see if it's already loaded
	{
		auto mod = moduleByFullName(name);
		if (mod != nullptr) {
			if (toFill != nullptr) { *toFill = mod; }
			return {};
		}
	}

	if (workspacePath().empty()) {
		res.addEntry("E52", "Cannot load module without a workspace path", {});
		return res;
	}

	// find it in the workspace
	fs::path fullPath = workspacePath() / "src" / name;
	fullPath.replace_extension(".chimod");

	if (!fs::is_regular_file(fullPath)) {
		res.addEntry("EUKN", "Failed to find module",
		             {{"Workspace Path", workspacePath().string()},
		              {"Expected Path", fullPath.generic_string()}});
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

	GraphModule* toFillJson = nullptr;
	res += addModuleFromJson(name.generic_string(), readJson, &toFillJson);
	if (toFill != nullptr) { *toFill = toFillJson; }

	// set this to the last time the file was edited
	toFillJson->updateLastEditTime(boost::filesystem::last_write_time(fullPath));

	return res;
}

Result Context::addModuleFromJson(const fs::path& fullName, const nlohmann::json& json,
                                  GraphModule** toFill) {
	Result res;

	auto scopedCtx = res.addScopedContext({{"Requested Module Name", fullName.string()}});

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

bool Context::addModule(std::unique_ptr<ChiModule> modToAdd) noexcept {
	assert(modToAdd != nullptr);

	// make sure it's unique
	auto ptr = moduleByFullName(modToAdd->fullName());
	if (ptr != nullptr) { return false; }

	if (modToAdd->fullName() == "lang") { mLangModule = dynamic_cast<LangModule*>(modToAdd.get()); }

	mModules.push_back(std::move(modToAdd));

	assert(modToAdd == nullptr);

	return true;
}

bool Context::unloadModule(const fs::path& fullName) {
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

Result Context::typeFromModule(const fs::path& module, boost::string_view name,
                               DataType* toFill) noexcept {
	assert(toFill != nullptr);

	Result res;

	ChiModule* mod = moduleByFullName(module);
	if (mod == nullptr) {
		res.addEntry("E36", "Could not find module", {{"module", module.generic_string()}});
		return res;
	}

	*toFill = mod->typeFromName(name);
	if (!toFill->valid()) {
		res.addEntry("E37", "Could not find type in module",
		             {{"type", name.to_string()}, {"module", module.generic_string()}});
	}

	return res;
}

Result Context::nodeTypeFromModule(const fs::path& moduleName, boost::string_view typeName,
                                   const nlohmann::json&      data,
                                   std::unique_ptr<NodeType>* toFill) noexcept {
	Result res;

	auto module = moduleByFullName(moduleName);
	if (module == nullptr) {
		res.addEntry("E36", "Could not find module", {{"module", moduleName.generic_string()}});
		return res;
	}

	res += module->nodeTypeFromName(typeName, data, toFill);

	return res;
}

Result Context::compileModule(const boost::filesystem::path& fullName,
                              Flags<CompileSettings>         settings,
                              std::unique_ptr<llvm::Module>* toFill) {
	Result res;

	auto mod = moduleByFullName(fullName);
	if (mod == nullptr) {
		res.addEntry("E36", "Could not find module", {{"module", fullName.generic_string()}});
		return res;
	}

	return compileModule(*mod, settings, toFill);
}

Result Context::compileModule(ChiModule& mod, Flags<CompileSettings> settings,
                              std::unique_ptr<llvm::Module>* toFill) {
	assert(toFill != nullptr);

	Result res;

	auto modNameCtx = res.addScopedContext({{"Module Name", mod.fullName()}});

	// generate module or load it from the cache
	std::unique_ptr<llvm::Module> llmod;
	{
		// try to get it from the cache
		if (settings & CompileSettings::UseCache) {
			llmod = moduleCache().retrieveFromCache(mod.fullNamePath(), mod.lastEditTime());
		}

		// compile it if the cache failed or if
		if (!llmod) {
			llmod = std::make_unique<llvm::Module>(mod.fullName(), llvmContext());

			// add forward declartions for all dependencies
			{
				std::unordered_set<fs::path> added(mod.dependencies().begin(),
				                                   mod.dependencies().end());
				std::deque<fs::path> depsToAdd(mod.dependencies().begin(),
				                               mod.dependencies().end());
				while (!depsToAdd.empty()) {
					auto& depName = depsToAdd[0];

					auto depMod = moduleByFullName(depName);
					if (depMod == nullptr) {
						res.addEntry("E36", "Could not find module",
						             {{"module", depName.generic_string()}});
						return res;
					}

					res += depMod->addForwardDeclarations(*llmod);
					if (!res) { return res; }

					for (const auto& depOfDep : depMod->dependencies()) {
						if (added.find(depOfDep) == added.end()) {
							depsToAdd.push_back(depOfDep);
							added.insert(depOfDep);
						}
					}
					depsToAdd.pop_front();
				}
			}

			res += mod.generateModule(*llmod);

			// set debug info version if it doesn't already have it
			if (llmod->getModuleFlag("Debug Info Version") == nullptr) {
				llmod->addModuleFlag(llvm::Module::Warning, "Debug Info Version",
				                     llvm::DEBUG_METADATA_VERSION);
			}
		}
	}

	// exit if there's already an error to avoid caching it
	if (!res) { return res; }

#ifndef NDEBUG

	// verify the created module
	bool        errored;
	std::string err;
	{
		llvm::raw_string_ostream os(err);
		errored = llvm::verifyModule(*llmod, &os);
	}

	if (errored) {
		// print out the module for the good errors
		std::string moduleStr;
		{
			llvm::raw_string_ostream printerStr{moduleStr};
			llmod->print(printerStr, nullptr);
		}

		res.addEntry("EINT", "Internal compiler error: Invalid module created",
		             {{"Error", err}, {"Full Name", mod.fullName()}, {"Module", moduleStr}});
	}
#endif

	// cache the module
	res += moduleCache().cacheModule(mod.fullNamePath(), *llmod, mod.lastEditTime());

	// generate dependencies
	if (settings & CompileSettings::LinkDependencies) {
		for (const auto& depName : mod.dependencies()) {
			std::unique_ptr<llvm::Module> compiledDep;
			res += compileModule(depName, settings,
			                     &compiledDep);  // TODO(#62): detect circular dependencies

			if (!res) { return res; }

// link it in
#if LLVM_VERSION_LESS_EQUAL(3, 7)
			llvm::Linker::LinkModules(llmod.get(), compiledDep.get()
#if LLVM_VERSION_LESS_EQUAL(3, 5)
			                                           ,
			                          llvm::Linker::DestroySource, nullptr
#endif
			                          );
#else
			llvm::Linker::linkModules(*llmod, std::move(compiledDep));
#endif
		}
	}

	*toFill = std::move(llmod);

	return res;
}

std::vector<NodeInstance*> Context::findInstancesOfType(const fs::path&    moduleName,
                                                        boost::string_view typeName) const {
	std::vector<NodeInstance*> ret;

	for (const auto& module : mModules) {
		// see if it's a GraphModule
		auto castedMod = dynamic_cast<GraphModule*>(module.get());
		if (castedMod == nullptr) { continue; }

		for (const auto& func : castedMod->functions()) {
			auto vec = func->nodesWithType(moduleName, typeName);
			std::copy(vec.begin(), vec.end(), std::back_inserter(ret));
		}
	}

	return ret;
}

fs::path workspaceFromChildPath(const boost::filesystem::path& path) {
	fs::path ret = path;

	// initialize workspace directory
	// go up until it is a workspace
	while (!ret.empty() && !fs::is_regular_file(ret / ".chigraphworkspace")) {
		ret = ret.parent_path();
	}

	return ret;  // it's ok if it's empty
}

std::string stringifyLLVMType(llvm::Type* ty) {
	assert(ty != nullptr);

	std::string data;
	{
		llvm::raw_string_ostream stream{data};
		ty->print(stream);
	}
	return data;
}

namespace {

std::unique_ptr<llvm::ExecutionEngine> createEE(std::unique_ptr<llvm::Module> mod,
                                                llvm::CodeGenOpt::Level       optLevel,
                                                std::string&                  errMsg) {
	llvm::InitializeNativeTarget();
	llvm::InitializeNativeTargetAsmPrinter();
	llvm::InitializeNativeTargetAsmParser();

	llvm::EngineBuilder EEBuilder(
#if LLVM_VERSION_LESS_EQUAL(3, 5)
	    mod.get()
#else
	    std::move(mod)
#endif

	        );

	EEBuilder.setEngineKind(llvm::EngineKind::JIT);

#ifndef _NDEBUG
	EEBuilder.setVerifyModules(true);
#endif

	EEBuilder.setOptLevel(optLevel);

	EEBuilder.setErrorStr(&errMsg);

#if LLVM_VERSION_LESS_EQUAL(3, 5)
	EEBuilder.setUseMCJIT(true);
#endif

	EEBuilder.setMCJITMemoryManager(
#if LLVM_VERSION_AT_LEAST(3, 6)
	    std::unique_ptr<llvm::SectionMemoryManager>
#endif
	    (new llvm::SectionMemoryManager()));

	return std::unique_ptr<llvm::ExecutionEngine>(EEBuilder.create());
}

}  // anonymous namespace

Result interpretLLVMIR(std::unique_ptr<llvm::Module> mod, llvm::CodeGenOpt::Level optLevel,
                       const std::vector<llvm::GenericValue>& args, llvm::Function* funcToRun,
                       llvm::GenericValue* ret) {
	assert(mod);

	Result res;

	if (funcToRun == nullptr) {
		funcToRun = mod->getFunction("main");

		if (funcToRun == nullptr) {
			res.addEntry("EUKN", "Failed to find main function in module",
			             {{"Module Name", mod->getModuleIdentifier()}});
			return res;
		}
	}

	std::string errMsg;
	auto        EE = createEE(std::move(mod), optLevel, errMsg);
	EE->finalizeObject();
	EE->runStaticConstructorsDestructors(false);

	if (!EE) {
		res.addEntry("EINT", "Failed to create an LLVM ExecutionEngine", {{"Error", errMsg}});
		return res;
	}

	auto returnValue = EE->runFunction(funcToRun, args);

	EE->runStaticConstructorsDestructors(true);

	if (ret != nullptr) { *ret = returnValue; }

	return res;
}

Result interpretLLVMIRAsMain(std::unique_ptr<llvm::Module> mod, llvm::CodeGenOpt::Level optLevel,
                             const std::vector<std::string>& args, llvm::Function* funcToRun,
                             int* ret) {
	assert(mod);

	Result res;

	if (funcToRun == nullptr) {
		funcToRun = mod->getFunction("main");

		if (funcToRun == nullptr) {
			res.addEntry("EUKN", "Failed to find main function in module",
			             {{"Module Name", mod->getModuleIdentifier()}});
			return res;
		}
	}

	std::string errMsg;
	auto        EE = createEE(std::move(mod), optLevel, errMsg);
	EE->finalizeObject();
	EE->runStaticConstructorsDestructors(false);

	if (!EE) {
		res.addEntry("EINT", "Failed to create an LLVM ExecutionEngine", {{"Error", errMsg}});
		return res;
	}

	auto returnValue = EE->runFunctionAsMain(funcToRun, args, nullptr);

	EE->runStaticConstructorsDestructors(true);

	if (ret != nullptr) { *ret = returnValue; }

	return res;
}
void Context::setModuleCache(std::unique_ptr<ModuleCache> newCache) {
	assert(newCache != nullptr && "Cannot set the modulecache to be nullptr");

	mModuleCache = std::move(newCache);
}

}  // namespace chi
