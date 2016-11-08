#include <chig/Context.hpp>
#include <chig/NodeType.hpp>

#include <boost/program_options.hpp>

#include <iostream>
#include <vector>
#include <string>

using namespace chig;

int main(int argc, char** argv) {
	
	namespace po = boost::program_options;
	
	po::options_description desc("chigc: Chigraph compiler command line parameters", 50);
	
	desc.add_options()
		("help,h", "Produce Help Message")
		("input-file", po::value<std::vector<std::string>>(), "Input files")
		("output", po::value<std::string>()->default_value("a.bc"), "Output File")
		;
	
	po::positional_options_description input_desc;
	input_desc.add("input-file", -1);
	
	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).options(desc).positional(input_desc).run(), vm);
	
	po::notify(vm);
	
	// see help
	if(vm.count("help")) {
		std::cout << desc << std::endl;
		return 0;
	}
	
	if(vm.count("input-file") == 0) {
		std::cerr << "error: no input files" << std::endl;
		return 1;
	}
	
	
	
}
