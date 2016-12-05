#include <vector>
#include <string>
#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include <git2.h>

namespace fs = boost::filesystem;
namespace po = boost::program_options;

int get(const std::vector<std::string>& opts)
{  
    git_libgit2_init();

	po::options_description get_opts("get options");
	get_opts.add_options()
      ("module", po::value<std::vector<std::string>>(), "Modules to get");

	po::positional_options_description pos;
	pos.add("module", -1);

	po::variables_map vm;
	po::store(po::command_line_parser(opts).options(get_opts).positional(pos).run(), vm);

	if (vm.count("module") == 0) {
      std::cerr << "Nothing to get!" << std::endl;
      return 1;
    }
    
    // get workspace dir
    fs::path workspaceDir = fs::current_path();
    while(!workspaceDir.empty() && !fs::is_regular_file(workspaceDir / ".chigraphworkspace")) {
      workspaceDir = workspaceDir.parent_path();
    }
    if(workspaceDir.empty()) {
      std::cerr << "Could not find workspace" << std::endl;
      return 1;
    }
    std::cout << "Workspace: " << workspaceDir << std::endl;
    
    auto mods = vm["module"].as<std::vector<std::string>>();
    
    for(auto& mod : mods) {
      
      std::string repoName = mod;
      std::string repoUrl = "https://" + repoName;
      
      git_repository* repo = nullptr;
      int err = git_clone(&repo,  repoUrl.c_str(), (workspaceDir / "src" / repoName).c_str(), nullptr);
      if(err != 0) {
        auto err = giterr_last();
        std::cerr << "Error cloning: " << err->klass << " Message: " << err->message << std::endl;
        return 1;
      }
      git_repository_free(repo);
      
    }
    
	return 0;
}

