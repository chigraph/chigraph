#include <chig/Context.hpp>
#include <chig/GraphFunction.hpp>
#include <chig/LangModule.hpp>
#include <chig/NodeType.hpp>
#include <chig/JsonModule.hpp>
#include <chig/Result.hpp>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include <llvm/Support/raw_os_ostream.h>

#include <iostream>
#include <string>
#include <vector>

using namespace chig;

int main(int argc, char** argv)
{
	namespace po = boost::program_options;
	namespace fs = boost::filesystem;

	po::options_description desc("chigc: Chigraph compiler command line parameters", 50);

	desc.add_options()("help,h", "Produce Help Message") // 
		("input-file", po::value<std::string>(), "Input File or - for stdin") // 
		("output,o", po::value<std::string>()->default_value("a.bc"), "Output File or - for stdout") // 
		;

	po::positional_options_description input_desc;
	input_desc.add("input-file", -1);

	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).options(desc).positional(input_desc).run(), vm);

	po::notify(vm);

	// see help
	if (vm.count("help")) {
		std::cout << desc << std::endl;
		return 0;
	}

	if (vm.count("input-file") == 0) {
		std::cerr << "error: no input files" << std::endl;
		return 1;
	}
	
	Context c;
	
	Result res;
	
	// compile files
	std::string infile = vm["input-file"].as<std::string>();
	
	nlohmann::json read_json;
	
	if(infile == "-") {
		std::cin >> read_json;
	} else {
	// make sure it's an actual file
		fs::path inpath = infile;
		if(!fs::is_regular_file(inpath)) {
			std::cerr << "error: Cannot open input file " << inpath;
			return 1;
		}
		
		fs::ifstream stream(inpath);
		stream >> read_json;
	}

	// load it as a module
	JsonModule module(read_json, c, &res);
	
	std::unique_ptr<llvm::Module> llmod;
	res += module.compile(&llmod);

	
	if(!res) {
		std::cerr << res.result_json.dump(2) << std::endl;
		return 1;
	}
	
	// create output
	if(vm.count("output")) {
		fs::ofstream outstream{vm["output"].as<std::string>()};
		
		llvm::raw_os_ostream llstream(outstream);
		llmod->print(llstream, nullptr);
	} else {
		
		llvm::raw_os_ostream llstream(std::cout);
		llmod->print(llstream, nullptr);
	}
		
	
}
