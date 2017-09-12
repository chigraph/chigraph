#include "chi/Fetcher/Fetcher.hpp"

#include <git2.h>

#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/operations.hpp>

namespace fs = boost::filesystem;

namespace chi {

Result fetchModule(const fs::path& workspacePath, const fs::path& name, bool recursive) {
	// init it (pretty sure it inits windows networking stuff)
	git_libgit2_init();

	Result res;

	auto modCtx = res.addScopedContext({{"Module Name", name.string()}});

	if (name == "lang") { return res; }

	// get the url
	std::string url;
	std::string cloneInto;
	VCSType     type;
	std::tie(type, url, cloneInto) = resolveUrlFromModuleName(name);

	// see if it exists
	auto fileName = workspacePath / "src" / fs::path(name).replace_extension(".chimod");
	bool exists   = fs::is_regular_file(fileName);

	if (exists) {
		// try to pull it

		// see if it's actually a git repository
		auto repoPath = workspacePath / "src" / cloneInto;
		if (cloneInto == "" || !fs::is_directory(repoPath / ".git")) {
			// it's not a git repo, just exit

			return res;
		}

		auto repoPathCtx = res.addScopedContext({{"Repo Path", repoPath.string()}});

		if (type == VCSType::Unknown) {
			res.addEntry("EUKN", "Could not resolve URL for module", {});
			return res;
		}
		assert(type == VCSType::Git && "Currently only Git is implemented for fetching modules.");

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
		    [](const char* name, const char* /*url*/, const git_oid* oid, unsigned int is_merge,
		       void* payload) -> int {
			    auto& oids_to_merge = *reinterpret_cast<std::pair<std::string, git_oid>*>(payload);

			    if (is_merge != 0u) { oids_to_merge = {name, *oid}; }

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

		if ((anaylisis & GIT_MERGE_ANALYSIS_UP_TO_DATE) != 0 ||
		    (anaylisis & GIT_MERGE_ANALYSIS_NONE) != 0) {
			// nothing to do, just return
			return res;
		}

		if ((anaylisis & GIT_MERGE_ANALYSIS_FASTFORWARD) != 0) {
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
			git_oid oid{};
			err = git_index_write_tree_to(&oid, head, repo);
			if (err != 0) {
				res.addEntry("EUKN", "Failed to write index to tree",
				             {{"Error Message", giterr_last()->message}});
				return res;
			}

		} else if ((anaylisis & GIT_MERGE_ANALYSIS_NORMAL) != 0) {
			// merge and commit
			git_merge_options    mergeOpts    = GIT_MERGE_OPTIONS_INIT;
			git_checkout_options checkoutOpts = GIT_CHECKOUT_OPTIONS_INIT;
			checkoutOpts.checkout_strategy    = GIT_CHECKOUT_SAFE;  // see
			// http://stackoverflow.com/questions/39651287/doing-a-git-pull-with-libgit2
			err = git_merge(repo, annotatedCommits, 1, &mergeOpts, &checkoutOpts);
			if (err != 0) {
				res.addEntry("EUKN", "Failed to merge branch",
				             {{"Error Message", giterr_last()->message}});
				return res;
			}

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
			git_oid parent_headoid{};
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

			git_oid     newCommit{};
			std::string commitMsg = std::string("Merge ") + git_oid_tostr_s(&oid_to_merge.second);
			err                   = git_commit_create(&newCommit, repo, "HEAD", committerSignature,
			                        committerSignature, "UTF-8", commitMsg.c_str(), tree,
			                        sizeof(parents) / sizeof(git_commit*),
			                        static_cast<const git_commit**>(parents));
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
		assert(type == VCSType::Git);

		auto absCloneInto = workspacePath / "src" / cloneInto;
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
	
	if (!fs::is_regular_file(fileName)) {
      res.addEntry("EUKN", "Module doesn't exist", {{"File Name", fileName.string()}});
      return res;
    }

	if (recursive) {
		// peek at the dependencies
		// TODO(#79): is there a cleaner way to do this?
		nlohmann::json j;
		try {
			fs::ifstream file{fileName};
			file >> j;
		} catch (std::exception& e) {
			res.addEntry("EUKN", "Failed to parse JSON",
			             {{"File", fileName.string()}, {"Error Message", e.what()}});
		}

		if (j.find("dependencies") != j.end() || !j["dependencies"].is_array()) { return res; }

		// fetch the dependencies
		for (const auto& dep : j["dependencies"]) {
			std::string depName = dep;
			fetchModule(workspacePath, depName, true);
		}
	}

	return res;
}

std::tuple<VCSType, std::string, std::string> resolveUrlFromModuleName(
    const boost::filesystem::path& path) {
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
