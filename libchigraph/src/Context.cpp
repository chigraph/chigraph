/// \file Context.cpp

#include "chi/Context.hpp"
#include "chi/GraphFunction.hpp"
#include "chi/GraphModule.hpp"
#include "chi/GraphStruct.hpp"
#include "chi/JsonDeserializer.hpp"
#include "chi/LLVMVersion.hpp"
#include "chi/LangModule.hpp"
#include "chi/NodeInstance.hpp"
#include "chi/Result.hpp"

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

#include <git2.h>

#include <gsl/gsl>

namespace fs = boost::filesystem;

namespace chi {
Context::Context(const fs::path& workPath) {
	mWorkspacePath = workspaceFromChildPath(workPath);

	git_libgit2_init();
}

Context::~Context() = default;

ChiModule* Context::moduleByFullName(const fs::path& fullModuleName) const noexcept {
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
		if (fs::is_regular_file(p) && p.extension() == ".chimod") {
			fs::path relPath = fs::relative(p, srcDir);

			relPath.replace_extension("");  // remove .chimod
			moduleList.emplace_back(relPath.string());
		}
	}

	return moduleList;
}

Result Context::loadModule(const fs::path& name, Flags<LoadSettings> settings, ChiModule** toFill) {
	Result res;

	auto requestedModCtx = res.addScopedContext({{"Requested Module Name", name.generic_string()}});

	if (settings & LoadSettings::Fetch) {
		res += fetchModule(name, bool(settings & LoadSettings::FetchRecursive));
		if (!res) { return res; }
	}

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

	if (workspacePath().empty()) {
		res.addEntry("E52", "Cannot load module without a workspace path", {});
		return res;
	}

	// find it in the workspace
	fs::path fullPath = workspacePath() / "src" / name;
	fullPath.replace_extension(".chimod");

	if (!fs::is_regular_file(fullPath)) {
		res.addEntry("EUKN", "Failed to find module",
		             {{"Workspace Path", workspacePath().string()}});
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

Result Context::fetchModule(const fs::path& name, bool recursive) {
	Result res;

	auto modCtx = res.addScopedContext({{"Module Name", name.string()}});

	if (name == "lang") { return res; }

	// get the url
	std::string url;
	std::string cloneInto;
	VCSType     type;
	std::tie(type, url, cloneInto) = resolveUrlFromModuleName(name);

	// see if it exists
	auto fileName = workspacePath() / "src" / fs::path(name).replace_extension(".chimod");
	bool exists   = fs::is_regular_file(fileName);

	if (exists) {
		// try to pull it

		// see if it's actually a git repository
		auto repoPath = workspacePath() / "src" / cloneInto;
		if (cloneInto == "" || !fs::is_directory(repoPath / ".git")) {
			// it's not a git repo, just exit

			return res;
		}

		auto repoPathCtx = res.addScopedContext({{"Repo Path", repoPath}});

		if (type == VCSType::Unknown) {
			res.addEntry("EUKN", "Could not resolve URL for module", {});
			return res;
		}
		Expects(type == VCSType::Git);

		// open the repository
		git_repository* repo;
		int             err = git_repository_open(&repo, repoPath.string().c_str());
		if (err != 0) {
			res.addEntry("EUKN", "Failed to open git repository",
			             {{"Error Message", giterr_last()->message}});
			return res;
		}

		// get the remote
		git_remote* origin;
		err = git_remote_lookup(&origin, repo, "origin");
		if (err != 0) {
			res.addEntry("EUKN", "Failed to get remote origin",
			             {{"Error Message", giterr_last()->message}});
			return res;
		}

		// fetch
		git_fetch_options opts = GIT_FETCH_OPTIONS_INIT;
		err                    = git_remote_fetch(origin, nullptr, &opts, nullptr);
		if (err != 0) {
			res.addEntry("EUKN", "Failed to fetch repo",
			             {{"Error Message", giterr_last()->message}});
			return res;
		}

		// get which heads we need to merge
		std::pair<std::string, git_oid> oid_to_merge;
		git_repository_fetchhead_foreach(
		    repo,
		    [](const char* name, const char* url, const git_oid* oid, unsigned int is_merge,
		       void* payload) -> int {
			    auto& oids_to_merge = *reinterpret_cast<std::pair<std::string, git_oid>*>(payload);

			    if (is_merge) { oids_to_merge = {name, *oid}; }

			    return 0;

			},
		    &oid_to_merge);

		// get origin/master
		git_annotated_commit* originmaster;
		err = git_annotated_commit_lookup(&originmaster, repo, &oid_to_merge.second);
		if (err != 0) {
			res.addEntry("EUKN", "Failed to get new head from repo",
			             {{"Error Message", giterr_last()->message}});
			return res;
		}

		auto annotatedCommits = const_cast<const git_annotated_commit**>(&originmaster);

		// see what we need to do
		git_merge_analysis_t   anaylisis;
		git_merge_preference_t pref;
		git_merge_analysis(&anaylisis, &pref, repo, annotatedCommits, 1);

		if (anaylisis & GIT_MERGE_ANALYSIS_UP_TO_DATE || anaylisis & GIT_MERGE_ANALYSIS_NONE) {
			// nothing to do, just return
			return res;
		}

		if (anaylisis & GIT_MERGE_ANALYSIS_FASTFORWARD) {
			// we can fast forward, do it

			// get master
			git_reference* master;
			err = git_repository_head(&master, repo);

			if (err != 0) {
				res.addEntry("EUKN", "Failed to get reference to master",
				             {{"Error Message", giterr_last()->message}});
				return res;
			}

			// fast forward
			git_reference* createdRef;
			err = git_reference_set_target(&createdRef, master, &oid_to_merge.second, "pull");
			if (err != 0) {
				res.addEntry("EUKN", "Failed to fast forward",
				             {{"Error Message", giterr_last()->message}});
				return res;
			}

			// get head
			git_index* head;
			err = git_repository_index(&head, repo);
			if (err != 0) {
				res.addEntry("EUKN", "Failed to get HEAD",
				             {{"Error Message", giterr_last()->message}});
				return res;
			}

			// reset to it
			git_oid oid;
			err = git_index_write_tree_to(&oid, head, repo);
			if (err != 0) {
				res.addEntry("EUKN", "Failed to write index to tree",
				             {{"Error Message", giterr_last()->message}});
				return res;
			}

		} else if (anaylisis & GIT_MERGE_ANALYSIS_NORMAL) {
			// merge and commit
			git_merge_options    mergeOpts    = GIT_MERGE_OPTIONS_INIT;
			git_checkout_options checkoutOpts = GIT_CHECKOUT_OPTIONS_INIT;
			checkoutOpts.checkout_strategy    = GIT_CHECKOUT_SAFE;  // see
			// http://stackoverflow.com/questions/39651287/doing-a-git-pull-with-libgit2
			err = git_merge(repo, annotatedCommits, 1, &mergeOpts, &checkoutOpts);

			// see if there are conflicts

			// get head
			git_index* head;
			err = git_repository_index(&head, repo);
			if (err != 0) {
				res.addEntry("EUKN", "Failed to get HEAD",
				             {{"Error Message", giterr_last()->message}});
				return res;
			}

			// check for conflicts
			if (git_index_has_conflicts(head) != 0) {
				// there are conflicts
				res.addEntry("WUKN", "Merge conflicts when pulling, manually resolve them.", {});
				return res;
			}

			// commit the merge

			// create a signature for this code
			git_signature* committerSignature;
			err = git_signature_now(&committerSignature, "Chigraph Fetch",
			                        "russellgreene8@gmail.com");
			if (err != 0) {
				res.addEntry("EUKN", "Failed to create git signature",
				             {{"Error Message", giterr_last()->message}});
				return res;
			}

			// get the origin/master commit
			git_commit* origin_master_commit;
			err = git_commit_lookup(&origin_master_commit, repo, &oid_to_merge.second);
			if (err != 0) {
				res.addEntry("EUKN", "Failed to get commit for origin/master",
				             {{"Error Message", giterr_last()->message}});
				return res;
			}

			// get the head commit
			git_oid parent_headoid;
			err = git_reference_name_to_id(&parent_headoid, repo, "HEAD");
			if (err != 0) {
				res.addEntry("EUKN", "Failed to get reference to HEAD",
				             {{"Error Message", giterr_last()->message}});
				return res;
			}

			git_commit* head_parent;
			err = git_commit_lookup(&head_parent, repo, &parent_headoid);
			if (err != 0) {
				res.addEntry("EUKN", "Failed to get commit from oid",
				             {{"Error Message", giterr_last()->message}});
				return res;
			}

			// get the tree
			git_tree* tree;
			err = git_commit_tree(&tree, head_parent);
			if (err != 0) {
				res.addEntry("EUKN", "Failed to git tree from commit",
				             {{"Error Message", giterr_last()->message}});
			}

			const git_commit* parents[] = {head_parent, origin_master_commit};

			git_oid     newCommit;
			std::string commitMsg = std::string("Merge ") + git_oid_tostr_s(&oid_to_merge.second);
			err =
			    git_commit_create(&newCommit, repo, "HEAD", committerSignature, committerSignature,
			                      "UTF-8", commitMsg.c_str(), tree, 2, parents);
			if (err != 0) {
				res.addEntry("EUKN", "Failed to create commit",
				             {{"Error Message", giterr_last()->message}});
			}
		}

		git_annotated_commit_free(originmaster);
		git_repository_state_cleanup(repo);

	} else {
		// doesn't exist, clone

		if (type == VCSType::Unknown) {
			res.addEntry("EUKN", "Could not resolve URL for module", {});
			return res;
		}
		Expects(type == VCSType::Git);

		auto absCloneInto = workspacePath() / "src" / cloneInto;
		// make sure the directory exists
		fs::create_directories(absCloneInto.parent_path());

		// clone it
		git_repository* repo;
		int             err = git_clone(&repo, url.c_str(), absCloneInto.string().c_str(), nullptr);

		// check for error
		if (err != 0) {
			res.addEntry("EUKN", "Failed to clone repository",
			             {{"Error Code", err}, {"Error Message", giterr_last()->message}});
			return res;
		}
	}

	if (recursive) {
		// peek at the dependencies
		// TODO: is there a cleaner way to do this?
		nlohmann::json j;
		try {
			fs::ifstream file{fileName};
			file >> j;
		} catch (std::exception& e) {
			res.addEntry("EUKN", "Failed to parse JSON",
			             {{"File", fileName}, {"Error Message", e.what()}});
		}

		if (j.find("dependencies") != j.end() || !j["dependencies"].is_array()) { return res; }

		// fetch the dependencies
		for (const auto& dep : j["dependencies"]) {
			std::string depName = dep;
			fetchModule(depName, true);
		}
	}

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
	Expects(modToAdd != nullptr);

	// make sure it's unique
	auto ptr = moduleByFullName(modToAdd->fullName());
	if (ptr != nullptr) { return false; }

	if (modToAdd->fullName() == "lang") { mLangModule = dynamic_cast<LangModule*>(modToAdd.get()); }

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

Result Context::compileModule(const fs::path& fullName, std::unique_ptr<llvm::Module>* toFill) {
	Result res;

	auto mod = moduleByFullName(fullName);
	if (mod == nullptr) {
		res.addEntry("E36", "Could not find module", {{"module", fullName.generic_string()}});
		return res;
	}

	return compileModule(*mod, toFill);
}

Result Context::compileModule(ChiModule& mod, std::unique_ptr<llvm::Module>* toFill) {
	Expects(toFill != nullptr);

	Result res;

	auto modNameCtx = res.addScopedContext({{"Module Name", mod.fullName()}});

	auto llmod = std::make_unique<llvm::Module>(mod.fullName(), llvmContext());

	// generate dependencies
	for (const auto& depName : mod.dependencies()) {
		std::unique_ptr<llvm::Module> compiledDep;
		res += compileModule(depName, &compiledDep);  // TODO(#62): detect circular dependencies

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

	llvm::EngineBuilder EEBuilder(
#if LLVM_VERSION_LESS_EQUAL(3, 5)
	    mod.get()
#else
	    std::move(mod)
#endif

	        );

	EEBuilder.setEngineKind(llvm::EngineKind::JIT);
	// EEBuilder.setVerifyModules(true);

	EEBuilder.setOptLevel(optLevel);

	EEBuilder.setErrorStr(&errMsg);

	EEBuilder.setMCJITMemoryManager(
#if LLVM_VERSION_AT_LEAST(3, 6)
	    std::unique_ptr<llvm::SectionMemoryManager>
#endif
	    (new llvm::SectionMemoryManager()));

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
                             std::vector<std::string> args, llvm::Function* funcToRun, int* ret) {
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

std::tuple<VCSType, std::string, std::string> resolveUrlFromModuleName(const fs::path& path) {
	// handle github
	{
		auto beginIter = path.begin();
		if (beginIter != path.end() && *beginIter == "github.com") {
			std::string folderName = beginIter->string();

			// get the url
			++beginIter;
			if (beginIter != path.end()) {
				folderName += "/";
				folderName += beginIter->string();
				++beginIter;
				if (beginIter != path.end()) {
					folderName += "/";
					folderName += beginIter->string();
				}
				return std::make_tuple(VCSType::Git, "https://" + folderName, folderName);
			}
		}
	}
	return std::make_tuple(VCSType::Unknown, "", "");
}

}  // namespace chi
