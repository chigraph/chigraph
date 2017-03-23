#include <vector>
#include <string>

#include <chi/Context.hpp>
#include <chi/Result.hpp>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

namespace po = boost::program_options;
namespace fs = boost::filesystem;

using namespace chi;

int get(const std::vector<std::string>& opts) {
	
	po::options_description get_options("get options");
	get_options.add_options()("module", po::value<std::vector<std::string>>(), "Modules to get")
	("workspace,w", po::value<std::string>()->default_value(fs::current_path().string()),
	                "The workspace path. Leave blank to inferr from the working directory");

	
	po::positional_options_description pos;
	pos.add("module", -1);
	
	
	po::variables_map vm;
	po::store(po::command_line_parser(opts).options(get_options).positional(pos).run(), vm);
	
	
	if (vm.count("module") == 0) {
		std::cerr << "no input files" << std::endl;
		return 1;
	}
	auto modules = vm["module"].as<std::vector<std::string>>();
	
	Context ctx{vm["workspace"].as<std::string>()};
	Result res;
	
	for(const auto& mod : modules) {
		res += ctx.fetchModule(mod, true);
		
	}
	
	std::cout << res << std::endl;
	if (!res) {
		return 1;
	}
	return 0;
}
