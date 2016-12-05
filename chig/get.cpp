#include <vector>
#include <string>
#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

namespace fs = boost::filesystem;
namespace po = boost::program_options;

int compile(const std::vector<std::string>& opts)
{
	po::options_description compile_opts("compile options");
	compile_opts.add_options()
      ("module", po::value<std::vector<std::string>>(), "Modules to get");

	po::positional_options_description pos;
	pos.add("module", -1);

	po::variables_map vm;
	po::store(po::command_line_parser(opts).options(compile_opts).positional(pos).run(), vm);

	if (vm.count("module") == 0) {
      std::cerr << "Nothing to get!";
    }
    
    
    
	return 0;
}

