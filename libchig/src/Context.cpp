/// \file Context.cpp

#include "chig/GraphFunction.hpp"
#include "chig/GraphModule.hpp"
#include "chig/GraphStruct.hpp"
#include "chig/JsonDeserializer.hpp"
#include "chig/LangModule.hpp"
#include "chig/NodeInstance.hpp"

#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/SectionMemoryManager.h>
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

#include <chig/CModule.hpp>
#include <gsl/gsl>
namespace fs = boost::filesystem;

namespace chig {
Context::Context(const fs::path& workPath) {
	mLLVMContext = std::make_unique<llvm::LLVMContext>();

	mWorkspacePath = workspaceFromChildPath(workPath);
}

ChigModule* Context::moduleByFullName(const fs::path& fullModuleName) const noexcept {
	Result res;

	for (auto& module : mModules) {
		if (module->fullName() == fullModuleName) { return module.get(); }
	}
	return nullptr;
}

GraphModule* Context::newGraphModule(const fs::path& fullName) {
	// create the module
	GraphModule* mod = nullptr;
	{
		auto uMod = std::make_unique<GraphModule>(*this, fullName, gsl::span<fs::path>());

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
		if (fs::is_regular_file(p) && p.extension() == ".chigmod") {
			fs::path relPath = fs::relative(p, srcDir);

			relPath.replace_extension("");  // remove .chigmod
			moduleList.emplace_back(relPath.string());
		}
	}

	return moduleList;
}

chig::Result chig::Context::loadModule(const fs::path& name, ChigModule** toFill) {
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
		res.addEntry("E52", "Cannot load module without a workspace path",
		             {{"Requested Module", name.generic_string()}});
		return res;
	}

	// find it in the workspace
	fs::path fullPath = workspacePath() / "src" / name;
	fullPath.replace_extension(".chigmod");

	if (!fs::is_regular_file(fullPath)) {
		res.addEntry(
		    "EUKN", "Failed to find module",
		    {{"Module Name", name.generic_string()}, {"Workspace Path", workspacePath().string()}});
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

	return res;
}

Result Context::addModuleFromJson(const fs::path& fullName, const nlohmann::json& json,
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
	Expects(toFill != nullptr);

	Result res;

	ChigModule* mod = moduleByFullName(module);
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
                              std::unique_ptr<llvm::Module>* toFill) {
	Result res;

	auto mod = moduleByFullName(fullName);
	if (mod == nullptr) {
		res.addEntry("E36", "Could not find module", {{"module", fullName.generic_string()}});
		return res;
	}

	return compileModule(*mod, toFill);
}

Result Context::compileModule(ChigModule& mod, std::unique_ptr<llvm::Module>* toFill) {
	Expects(toFill != nullptr);

	Result res;

	auto llmod = std::make_unique<llvm::Module>(mod.fullName(), llvmContext());

	// generate dependencies
	for (const auto& depName : mod.dependencies()) {
		std::unique_ptr<llvm::Module> compiledDep;
		res += compileModule(depName, &compiledDep);  // TODO(#62): detect circular dependencies

		if (!res) { return res; }

// link it in
#if LLVM_VERSION_MAJOR <= 3 && LLVM_VERSION_MINOR <= 7
		llvm::Linker::LinkModules(llmod.get(), compiledDep.get());
#else
		llvm::Linker::linkModules(*llmod, std::move(compiledDep));
#endif
	}

	res += mod.generateModule(*llmod);

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

			res.addEntry("EINT", "Internal compiler error: Invalid module created",
			             {{"Error", err}, {"Full Name", mod.fullName()}, {"Module", moduleStr}});
		}
	}

	*toFill = std::move(llmod);

	return res;
}

std::vector<NodeInstance*> Context::findInstancesOfType(const boost::filesystem::path& moduleName,
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

fs::path workspaceFromChildPath(const fs::path& path) {
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

namespace {

std::unique_ptr<llvm::ExecutionEngine> createEE(std::unique_ptr<llvm::Module> mod,
                                                llvm::CodeGenOpt::Level       optLevel,
                                                std::string&                  errMsg) {
	llvm::InitializeNativeTarget();
	llvm::InitializeNativeTargetAsmPrinter();
	llvm::InitializeNativeTargetAsmParser();

	llvm::EngineBuilder EEBuilder(std::move(mod));

	EEBuilder.setEngineKind(llvm::EngineKind::JIT);
	EEBuilder.setVerifyModules(true);

	EEBuilder.setOptLevel(optLevel);

	EEBuilder.setErrorStr(&errMsg);

	EEBuilder.setMCJITMemoryManager(std::make_unique<llvm::SectionMemoryManager>());

	return std::unique_ptr<llvm::ExecutionEngine>(EEBuilder.create());
}

}  // anonymous namespace

Result interpretLLVMIR(std::unique_ptr<llvm::Module> mod, llvm::CodeGenOpt::Level optLevel,
                       std::vector<llvm::GenericValue> args, llvm::Function* funcToRun,
                       llvm::GenericValue* ret) {
	Result res;

	if (funcToRun == nullptr) {
		funcToRun = mod->getFunction("main");

		if (funcToRun == nullptr) {
			res.addEntry("EUKN", "Failed to find main function in module",
			             {{"Module Name", mod->getName()}});
			return res;
		}
	}

	std::string errMsg;
	auto        EE = createEE(std::move(mod), optLevel, errMsg);

	if (!EE) {
		res.addEntry("EINT", "Failed to create an LLVM ExecutionEngine", {{"Error", errMsg}});
		return res;
	}

	auto returnValue = EE->runFunction(funcToRun, args);

	if (ret != nullptr) { *ret = returnValue; }

	return res;
}

Result interpretLLVMIRAsMain(std::unique_ptr<llvm::Module> mod, llvm::CodeGenOpt::Level optLevel,
                             std::vector<std::string> args, llvm::Function* funcToRun, int* ret) {
	Result res;

	if (funcToRun == nullptr) {
		funcToRun = mod->getFunction("main");

		if (funcToRun == nullptr) {
			res.addEntry("EUKN", "Failed to find main function in module",
			             {{"Module Name", mod->getName()}});
			return res;
		}
	}

	std::string errMsg;
	auto        EE = createEE(std::move(mod), optLevel, errMsg);

	if (!EE) {
		res.addEntry("EINT", "Failed to create an LLVM ExecutionEngine", {{"Error", errMsg}});
		return res;
	}

	auto returnValue = EE->runFunctionAsMain(funcToRun, args, nullptr);

	if (ret != nullptr) { *ret = returnValue; }

	return res;
}

}  // namespace chig
