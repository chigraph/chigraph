#include <string>
#include <vector>

#include <chi/Context.hpp>
#include <chi/Fetcher/Fetcher.hpp>
#include <chi/Support/Result.hpp>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

namespace po = boost::program_options;
namespace fs = boost::filesystem;

using namespace chi;

int get(const std::vector<std::string>& opts) {
	po::options_description get_options("chi get");

	// clang-format off
	get_options.add_options()
		("module", po::value<std::vector<std::string>>(), "Modules to get")
		("workspace,w", po::value<std::string>()->default_value(fs::current_path().string()), "The workspace path. Leave blank to inferr from the working directory")
		;
	// clang-format on

	po::positional_options_description pos;
	pos.add("module", -1);

	po::variables_map vm;
	po::store(po::command_line_parser(opts).options(get_options).positional(pos).run(), vm);

	if (vm.count("module") == 0) {
		std::cerr << "no input files" << std::endl;
		return 1;
	}
	auto modules = vm["module"].as<std::vector<std::string>>();

	auto workspacePath = workspaceFromChildPath(vm["workspace"].as<std::string>());

	if (workspacePath.empty()) {
		std::cerr << "Workspace path not an actual workspace" << std::endl;
		return 1;
	}

	Result res;

	for (const auto& mod : modules) { res += fetchModule(workspacePath, mod, true); }

	std::cout << res << std::endl;
	if (!res) { return 1; }
	return 0;
}
