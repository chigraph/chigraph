
#include <vector>
#include <string>

#include <boost/program_options.hpp>

namespace po = boost::program_options;

int compile(const std::vector<std::string> opts) {
	
	po::options_description compile_opts("compile options");
	compile_opts.add_options()
		("input-file", po::value<std::string>(), "Input file")
		;
	
	
	return 0;
} 
