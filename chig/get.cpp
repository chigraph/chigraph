#include <iostream>
#include <string>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include <git2.h>

#include <chig/Context.hpp>

namespace fs = boost::filesystem;
namespace po = boost::program_options;

int get(const std::vector<std::string>& opts)
{
	git_libgit2_init();

	po::options_description get_opts("get options");
	get_opts.add_options()("module", po::value<std::vector<std::string>>(), "Modules to get")(
		"workspace,w", po::value<std::string>(),
		"The workspace to use, use working directory if omitted");

	po::positional_options_description pos;
	pos.add("module", -1);

	po::variables_map vm;
	po::store(po::command_line_parser(opts).options(get_opts).positional(pos).run(), vm);

	if (vm.count("module") == 0) {
		std::cerr << "Nothing to get!" << std::endl;
		return 0;
	}

	// get workspace dir
	fs::path workspaceDir;
	if (vm.count("workspace") != 0) {
		workspaceDir = vm["workspace"].as<std::string>();
	} else {
		workspaceDir = fs::current_path();
	}
	workspaceDir = chig::workspaceFromChildPath(workspaceDir);
	if (workspaceDir.empty()) {
		std::cerr << "Directory \"" << workspaceDir
				  << "\" isn't in a workspace (ie. no .chigraphworkspace file found" << std::endl;
		return 1;
	}
	std::cout << "Workspace: " << workspaceDir << std::endl;

	auto mods = vm["module"].as<std::vector<std::string>>();

	for (auto& mod : mods) {
		fs::path modulePath = workspaceDir / "src" / (mod + ".chigmod");

		// if it already exists, then just git pull
		if (fs::is_regular_file(modulePath)) {
		}

		std::string repoName = mod;
		std::string repoUrl = "https://" + repoName;

		git_repository* repo = nullptr;
		int err =
			git_clone(&repo, repoUrl.c_str(), (workspaceDir / "src" / repoName).c_str(), nullptr);
		if (err != 0) {
			auto err = giterr_last();
			std::cerr << "Error cloning: " << err->klass << " Message: " << err->message
					  << std::endl;
			return 1;
		}
		git_repository_free(repo);
	}

	return 0;
}
