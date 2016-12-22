#include "chig/Context.hpp"
#include "chig/GraphFunction.hpp"
#include "chig/JsonModule.hpp"
#include "chig/LangModule.hpp"
#include "chig/NodeType.hpp"

#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Linker/Linker.h>
#include <llvm/Support/raw_ostream.h>

#include <boost/filesystem.hpp>

#include <chig/CModule.hpp>
#include <gsl/gsl>

using namespace llvm;

namespace fs = boost::filesystem;

namespace chig
{
Context::Context(const fs::path& workPath)
{
	mLLVMContext = std::make_unique<llvm::LLVMContext>();

	mWorkspacePath = workspaceFromChildPath(workPath);
}
ChigModule* Context::moduleByName(gsl::cstring_span<> moduleName) const noexcept
{
	Result res;

	for (auto& module : mModules) {
		if (module->name() == moduleName) {
			return module.get();
		}
	}
	return nullptr;
}

ChigModule* Context::moduleByFullName(gsl::cstring_span<> fullModuleName) const noexcept
{
	Result res;

	for (auto& module : mModules) {
		if (module->fullName() == fullModuleName) {
			return module.get();
		}
	}
	return nullptr;
}

chig::Result chig::Context::loadModule(const gsl::cstring_span<> name, ChigModule** toFill)
{
	Result res;


	// check for built-in modules
	if (name == "lang") {
		auto mod = std::make_unique<LangModule>(*this);
		if (toFill) {
			*toFill = mod.get();
		}
		addModule(std::move(mod));
		return {};
	}
	if (name == "c") {
		auto mod = std::make_unique<CModule>(*this);
		if (toFill) {
			*toFill = mod.get();
		}
		addModule(std::move(mod));  // we don't care if it's actually added
		return {};
	}
	
	
	if (workspacePath().empty()) {
        res.addEntry("EUKN", "Cannot load module without a workspace path", {{"Requested Module"}, gsl::to_string(name)});
		return res; 
	}

	// find it in the workspace
	fs::path fullPath = workspacePath() / "src" / (gsl::to_string(name) + ".chigmod");

	if (!fs::is_regular_file(fullPath)) {
		res.addEntry("EUKN", "Failed to find module",
			{{"Module Name", gsl::to_string(name)}, {"Workspace Path", workspacePath().string()}});
		return res;
	}

	// load the JSON
	nlohmann::json readJson = {};
	{
		fs::ifstream inFile{fullPath};

		inFile >> readJson;
	}

	JsonModule* toFillJson;
	res += addModuleFromJson(name, readJson, &toFillJson);
	if (toFill) {
		*toFill = toFillJson;
	}

	return res;
}

Result Context::addModuleFromJson(
	gsl::cstring_span<> fullName, const nlohmann::json& json, JsonModule** toFill)
{
	Result res;

	// make sure it's not already added
	{
		auto mod = moduleByFullName(fullName);
		if (mod != nullptr) {
			if (toFill) {
				auto casted = dynamic_cast<JsonModule*>(mod);
				if (casted) {
					*toFill = casted;
				}
			}
			return {};
		}
	}

	// Create the module
	JsonModule* cPtr = nullptr;
	{
		// parse module
		auto jmod = std::make_unique<JsonModule>(*this, gsl::to_string(fullName), json, &res);
		if (!res) {
			return res;
		}
		if (toFill != nullptr) {
			*toFill = jmod.get();
		}

		cPtr = jmod.get();
		bool added = addModule(std::move(jmod));
		Expects(added);  // it really should be added
	}
	if (!res) {
		return res;
	}

	// load graphs
	res += cPtr->loadGraphs();

	return res;
}

bool Context::addModule(std::unique_ptr<ChigModule> modToAdd) noexcept
{
	Expects(modToAdd != nullptr);
    
	// make sure it's unique
	auto ptr = moduleByFullName(modToAdd->fullName());
	if (ptr != nullptr) {
        return false;
	}

	mModules.push_back(std::move(modToAdd));

	Expects(modToAdd == nullptr);

	return true;
}

Result Context::typeFromModule(
	gsl::cstring_span<> module, gsl::cstring_span<> name, DataType* toFill) noexcept
{
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
	const nlohmann::json& data, std::unique_ptr<NodeType>* toFill) noexcept
{
	Result res;

	auto module = moduleByName(moduleName);
	if (module == nullptr) {
		res.addEntry("E36", "Could not find module", {{"module", gsl::to_string(moduleName)}});
		return res;
	}

	res += module->nodeTypeFromName(typeName, data, toFill);

	return res;
}

Result Context::compileModule(gsl::cstring_span<> fullName, std::unique_ptr<llvm::Module>* toFill)
{
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

		if (!res) {
			return res;
		}

		// link it in
		llvm::Linker::linkModules(*llmod, std::move(compiledDep));
	}

	res += chigmod->generateModule(&llmod);

	// verify the created module
	if (res) {
		std::string err;
		llvm::raw_string_ostream os(err);
		if (llvm::verifyModule(*llmod, &os)) {
			res.addEntry(
				"EUKN", "Internal compiler error: Invalid module created", {{"Error", err}});
		}
	}

	*toFill = std::move(llmod);

	return res;
}

std::string Context::fullModuleName(gsl::cstring_span<> shortName) const
{
	auto mod = moduleByName(shortName);

	if (mod != nullptr) {
		return mod->fullName();
	}

	return "";
}
fs::path workspaceFromChildPath(const fs::path& path)
{
	fs::path ret = path;

	// initialize workspace directory
	// go up until it is a workspace
	while (!ret.empty() && !fs::is_regular_file(ret / ".chigraphworkspace")) {
		ret = ret.parent_path();
	}

	return ret;  // it's ok if it's empty
}

std::string stringifyLLVMType(llvm::Type* ty)
{
	Expects(ty != nullptr);

	std::string data;
	{
		llvm::raw_string_ostream stream{data};
		ty->print(stream);
	}
	return data;
}

}  // namespace chig
