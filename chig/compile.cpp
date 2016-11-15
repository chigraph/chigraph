#include <vector>
#include <iostream>
#include <string>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include <chig/JsonModule.hpp>
#include <chig/Context.hpp>
#include <chig/Result.hpp>
#include <chig/json.hpp>
#include <chig/NodeType.hpp>
#include <chig/GraphFunction.hpp>

#include <llvm/Support/raw_os_ostream.h>
#include <llvm/IR/Module.h>

using namespace chig;

namespace fs = boost::filesystem;
namespace po = boost::program_options;

int compile(const std::vector<std::string> opts) {
	
	po::options_description compile_opts("compile options");
	compile_opts.add_options()
		("input-file", po::value<std::string>(), "Input file, - for stdin")
		("output,o", po::value<std::string>(), "Output file, - for stdout")
		;
	
	po::positional_options_description pos;
	pos.add("input-file", 1);
	
	po::variables_map vm;
	po::store(po::command_line_parser(opts).options(compile_opts).run(), vm);
	
	if(vm.count("input-file") == 0) {
		std::cerr << "chig compile: error: no input files" << std::endl;
		return 1;
	}
	
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

	Result res;
	Context c;
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

	
	return 0;
} 
