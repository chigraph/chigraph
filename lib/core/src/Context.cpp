/// \file Context.cpp

#include "chi/Context.hpp"

#include <llvm-c/Analysis.h>
#include <llvm-c/Core.h>
#include <llvm-c/DebugInfo.h>
#include <llvm-c/IRReader.h>
#include <llvm-c/Linker.h>
#include <llvm-c/Target.h>

#include <boost/algorithm/string/replace.hpp>
#include <boost/range.hpp>
#include <deque>
#include <filesystem>
#include <fstream>
#include <unordered_set>

#include "chi/BitcodeParser.hpp"
#include "chi/DefaultModuleCache.hpp"
#include "chi/GraphFunction.hpp"
#include "chi/GraphModule.hpp"
#include "chi/GraphStruct.hpp"
#include "chi/JsonDeserializer.hpp"
#include "chi/LangModule.hpp"
#include "chi/NodeInstance.hpp"
#include "chi/NodeType.hpp"
#include "chi/Owned.hpp"
#include "chi/Support/ExecutablePath.hpp"
#include "chi/Support/Result.hpp"

namespace fs = std::filesystem;

namespace chi {

namespace {
Result verifyModuleIfDebug(LLVMModuleRef module) {
	Result res;
#ifndef NDEBUG

	auto moduleStr = OwnedMessage(LLVMPrintModuleToString(module));

	// weirdness here. LLVM was reporting false positives (errored for valid modules)
	// and reserializing helped. go figure.
	auto buffer =
	    LLVMCreateMemoryBufferWithMemoryRange(*moduleStr, strlen(*moduleStr), nullptr, false);

	OwnedLLVMModule module2;
	bool failed = LLVMParseIRInContext(LLVMGetModuleContext(module), buffer, &*module2, nullptr);

	if (failed) {
		res.addEntry("EINT", "Internal compiler error: could not read compiled module",
		             {{"Module", *moduleStr}});
		return res;
	}

	// verify the created module
	OwnedMessage errorMessage;
	bool         errored = LLVMVerifyModule(*module2, LLVMReturnStatusAction, &*errorMessage);

	if (errored) {
		res.addEntry("EINT", "Internal compiler error: Invalid module created",
		             {{"Error", *errorMessage}, {"Module", *moduleStr}});
	}
#endif
	return res;
}
}  // namespace

Context::Context(const std::filesystem::path& workPath) {
	mWorkspacePath = workspaceFromChildPath(workPath);

	mModuleCache = std::make_unique<DefaultModuleCache>(*this);

	mLLVMContext = OwnedLLVMContext(LLVMContextCreate());
}

Context::~Context() = default;

ChiModule* Context::moduleByFullName(const std::filesystem::path& fullModuleName) const noexcept {
	for (auto& module : mModules) {
		if (module->fullName() == fullModuleName) { return module.get(); }
	}
	return nullptr;
}

GraphModule* Context::newGraphModule(const std::filesystem::path& fullName) {
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
	         fs::recursive_directory_iterator{srcDir,
	                                          fs::directory_options::follow_directory_symlink},
	         {})) {
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
		std::ifstream inFile{fullPath};

		inFile >> readJson;
	} catch (std::exception& e) {
		res.addEntry("EUKN", "Failed to parse json", {{"Error", e.what()}});
		return res;
	}

	GraphModule* toFillJson = nullptr;
	res += addModuleFromJson(name.generic_string(), readJson, &toFillJson);
	if (toFill != nullptr) { *toFill = toFillJson; }

	// set this to the last time the file was edited
	toFillJson->updateLastEditTime(std::filesystem::last_write_time(fullPath));

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

	// add the converter nodes
	for (const auto& tyName : modToAdd->nodeTypeNames()) {
		// create it
		std::unique_ptr<NodeType> ty;
		auto                      res = modToAdd->nodeTypeFromName(tyName, {}, &ty);

		if (!res) {
			// converter nodes must be stateless
			continue;
		}

		if (!ty->converter()) { continue; }

		// add it!
		mTypeConverters[ty->dataInputs()[0].type.qualifiedName()]
		               [ty->dataOutputs()[0].type.qualifiedName()] = std::move(ty);
	}

	mModules.push_back(std::move(modToAdd));

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

Result Context::typeFromModule(const fs::path& module, std::string_view name,
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
		             {{"type", name}, {"module", module.generic_string()}});
	}

	return res;
}

Result Context::nodeTypeFromModule(const fs::path& moduleName, std::string_view typeName,
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

std::unique_ptr<NodeType> Context::createConverterNodeType(const DataType& fromType,
                                                           const DataType& toType) {
	auto fromIter = mTypeConverters.find(fromType.qualifiedName());
	if (fromIter == mTypeConverters.end()) { return nullptr; }

	auto toIter = fromIter->second.find(toType.qualifiedName());
	if (toIter == fromIter->second.end()) { return nullptr; }

	return toIter->second->clone();
}

Result Context::compileModule(const std::filesystem::path& fullName,
                              Flags<CompileSettings> settings, OwnedLLVMModule* toFill) {
	Result res;

	auto mod = moduleByFullName(fullName);
	if (mod == nullptr) {
		res.addEntry("E36", "Could not find module", {{"module", fullName.generic_string()}});
		return res;
	}

	return compileModule(*mod, settings, toFill);
}

Result Context::compileModule(ChiModule& mod, Flags<CompileSettings> settings,
                              OwnedLLVMModule* toFill) {
	assert(toFill != nullptr);

	Result res;

	auto modNameCtx = res.addScopedContext({{"Module Name", mod.fullName()}});

	// generate module or load it from the cache
	OwnedLLVMModule llmod;
	{
		// try to get it from the cache
		if (settings & CompileSettings::UseCache) {
			llmod = moduleCache().retrieveFromCache(mod.fullNamePath(), mod.lastEditTime());
		}

		// compile it if the cache failed or if
		if (!llmod) {
			llmod = OwnedLLVMModule(
			    LLVMModuleCreateWithNameInContext(mod.fullName().c_str(), llvmContext()));

			// add forward declartions for all dependencies
			{
				std::unordered_set<fs::path> added(mod.dependencies().begin(),
				                                   mod.dependencies().end());
				std::deque<fs::path>         depsToAdd(mod.dependencies().begin(),
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
			const char* DIVKey = "Debug Info Version";
			if (LLVMGetModuleFlag(*llmod, DIVKey, strlen(DIVKey)) == nullptr) {
				LLVMAddModuleFlag(*llmod, LLVMModuleFlagBehaviorWarning, DIVKey, strlen(DIVKey),
				                  LLVMValueAsMetadata(constI32(LLVMDebugMetadataVersion())));
			}
		}
	}

	// exit if there's already an error to avoid caching it
	if (!res) { return res; }

	res += verifyModuleIfDebug(*llmod);
	if (!res) { return res; }

	// cache the module
	res += moduleCache().cacheModule(mod.fullNamePath(), *llmod, mod.lastEditTime());

	// generate dependencies
	if (settings & CompileSettings::LinkDependencies) {
		for (const auto& depName : mod.dependencies()) {
			OwnedLLVMModule compiledDep;
			res += compileModule(depName, settings,
			                     &compiledDep);  // TODO(#62): detect circular dependencies

			if (!res) { return res; }

			if (LLVMLinkModules2(*llmod, compiledDep.take_ownership())) {
				res.addEntry("EINT", "Failed to link modules", {});
				return res;
			}
		}

		// link in runtime if this is a main module
		if (mod.shortName() == "main") {
			// find the runtime
			auto runtimebc =
			    executablePath().parent_path().parent_path() / "lib" / "chigraph" / "runtime.bc";

			// just in case the executable is in a "Debug" folder or something
			if (!fs::is_regular_file(runtimebc)) {
				runtimebc = executablePath().parent_path().parent_path().parent_path() / "lib" /
				            "chigraph" / "runtime.bc";
			}

			if (!fs::is_regular_file(runtimebc)) {
				res.addEntry(
				    "EUKN", "Failed to find runtime.bc in lib/chigraph/runtime.bc",
				    {{"Install prefix", executablePath().parent_path().parent_path().string()}});
			}

			// load the BC file
			OwnedLLVMModule runtimeMod;
			res += parseBitcodeFile(runtimebc, llvmContext(), &runtimeMod);
			if (!res) { return res; }

			if (LLVMLinkModules2(*llmod, runtimeMod.take_ownership())) {
				res.addEntry("EINT", "Failed to link modules", {});
				return res;
			}

			res += verifyModuleIfDebug(*llmod);
			if (!res) return res;
		}
	}

	*toFill = std::move(llmod);

	return res;
}

std::vector<NodeInstance*> Context::findInstancesOfType(const fs::path&  moduleName,
                                                        std::string_view typeName) const {
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

void Context::setModuleCache(std::unique_ptr<ModuleCache> newCache) {
	assert(newCache != nullptr && "Cannot set the modulecache to be nullptr");

	mModuleCache = std::move(newCache);
}

LLVMValueRef Context::constI32(int32_t value) {
	return LLVMConstInt(LLVMInt32TypeInContext(llvmContext()), value, false);
}

LLVMValueRef Context::constF64(double value) {
	return LLVMConstReal(LLVMDoubleTypeInContext(llvmContext()), value);
}

LLVMValueRef Context::constBool(bool value) { return LLVMConstInt(LLVMInt1Type(), value, false); }

fs::path workspaceFromChildPath(const fs::path& path) {
	fs::path ret;
	try {
		ret = fs::absolute(path);
	} catch (const fs::filesystem_error& /*e*/) { ret = fs::current_path(); }

	// initialize workspace directory
	// go up until it is a workspace
	while (ret != ret.root_path()) {
		if (fs::is_regular_file(ret / ".chigraphworkspace")) { return ret; }
		ret = ret.parent_path();
	}

	return fs::path();  // did not find a .chigraphworkspace
}

std::string stringifyLLVMType(LLVMTypeRef ty) {
	assert(ty != nullptr);

	return *OwnedMessage(LLVMPrintTypeToString(ty));
}

namespace {

OwnedLLVMExecutionEngine createEE(OwnedLLVMModule mod, LLVMCodeGenOptLevel optLevel,
                                  std::string& errMsg) {
	bool failed = LLVMInitializeNativeTarget();
	failed      = failed || LLVMInitializeNativeAsmPrinter();
	failed      = failed || LLVMInitializeNativeAsmParser();

	// this only failes if it wasn't compiled with it, in theory
	if (failed) {
		errMsg = "Failed to initialize LLVM JIT";
		return nullptr;
	} 

	OwnedLLVMExecutionEngine engine;
	OwnedMessage             llErrMsg;
	if (LLVMCreateJITCompilerForModule(&*engine, mod.take_ownership(), optLevel, &*llErrMsg)) {
		errMsg = *llErrMsg;
		return nullptr;
	}
	assert(engine);

		return engine;
	}

}  // anonymous namespace

Result interpretLLVMIR(OwnedLLVMModule mod, LLVMCodeGenOptLevel optLevel,
                       std::vector<LLVMGenericValueRef> args, LLVMValueRef funcToRun,
                       LLVMGenericValueRef* ret) {
	assert(mod);

	Result res;

	if (funcToRun == nullptr) {
		funcToRun = LLVMGetNamedFunction(*mod, "main");

		if (funcToRun == nullptr) {
			size_t      name_len;
			const char* id = LLVMGetModuleIdentifier(*mod, &name_len);

			res.addEntry("EUKN", "Failed to find main function in module",
			             {{"Module Name", std::string(id, name_len)}});
			return res;
		}
	}

	std::string errMsg;
	auto        EE = createEE(std::move(mod), optLevel, errMsg);
	if (!EE) {
		res.addEntry("EINT", "Failed to create an LLVM ExecutionEngine", {{"Error", errMsg}});
		return res;
	}
	LLVMRunStaticConstructors(*EE);

	auto returnValue = LLVMRunFunction(*EE, funcToRun, args.size(), &args[0]);

	LLVMRunStaticDestructors(*EE);

	if (ret != nullptr) { *ret = returnValue; }

	return res;
}

Result interpretLLVMIRAsMain(OwnedLLVMModule mod, LLVMCodeGenOptLevel optLevel,
                             const std::vector<std::string>& args, LLVMValueRef funcToRun,
                             int* ret) {
	assert(mod);

	Result res;

	if (funcToRun == nullptr) {
		funcToRun = LLVMGetNamedFunction(*mod, "main");

		if (funcToRun == nullptr) {
			size_t      name_len;
			const char* id = LLVMGetModuleIdentifier(*mod, &name_len);

			res.addEntry("EUKN", "Failed to find main function in module",
			             {{"Module Name", std::string(id, name_len)}});
			return res;
		}
	}

	std::string errMsg;
	auto        EE = createEE(std::move(mod), optLevel, errMsg);
	if (!EE) {
		res.addEntry("EINT", "Failed to create an LLVM ExecutionEngine", {{"Error", errMsg}});
		return res;
	}
	LLVMRunStaticConstructors(*EE);

	std::vector<const char*> argv;
	std::transform(args.begin(), args.end(), std::back_inserter(argv),
	               [](const std::string& str) { return str.c_str(); });
	auto returnValue = LLVMRunFunctionAsMain(*EE, funcToRun, argv.size(), argv.data(), nullptr);

	LLVMRunStaticDestructors(*EE);

	if (ret != nullptr) { *ret = returnValue; }

	return res;
}
}  // namespace
