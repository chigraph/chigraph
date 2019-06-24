#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <boost/program_options.hpp>

#include <chi/Context.hpp>

extern int get(const std::vector<std::string>& opts);

namespace po = boost::program_options;
namespace fs = std::filesystem;

int init(const std::vector<std::string>& opts) {
	po::options_description init_ops("init options");
	init_ops.add_options()("no-hello-world", "Do not fetch the hello world module");

	auto options = po::command_line_parser(opts).options(init_ops).run();

	auto workspacePath = chi::workspaceFromChildPath(fs::current_path());
	if (!workspacePath.empty()) {
		std::cerr << "Already in a chigraph workspace! Exiting..." << std::endl;
		return 1;
	}

	// make the chigraph workspace file
	{ std::ofstream workspacefile(".chigraphworkspace"); }

	// create the src/ directory
	fs::create_directory("src");

	// get the hellochigrpah module
	return get({"github.com/chigraph/hellochigraph/hello/main"});
}
