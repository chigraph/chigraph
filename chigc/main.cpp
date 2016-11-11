#include <chig/Context.hpp>
#include <chig/GraphFunction.hpp>
#include <chig/LangModule.hpp>
#include <chig/NodeType.hpp>

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
		("input-files", po::value<std::vector<std::string>>(), "Input Files") // 
		("output,o", po::value<std::string>()->default_value("a.bc"), "Output File") // 
		;

	po::positional_options_description input_desc;
	input_desc.add("input-files", -1);

	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).options(desc).positional(input_desc).run(), vm);

	po::notify(vm);

	// see help
	if (vm.count("help")) {
		std::cout << desc << std::endl;
		return 0;
	}

	if (vm.count("input-files") == 0) {
		std::cerr << "error: no input files" << std::endl;
		return 1;
	}
	
	Context c;
	
	// create module for the functions
	auto llmod = std::make_unique<llvm::Module>("main", c.context);
	
	Result res;
	
	// compile files
	for(auto& infile : vm["input-files"].as<std::vector<std::string>>()) {
		// make sure it's an actual file
		fs::path inpath = infile;
		if(!fs::is_regular_file(inpath)) {
			std::cerr << "error: Cannot open input file " << inpath;
			return 1;
		}
		
		
		nlohmann::json read_json;
		fs::ifstream stream(inpath);
		stream >> read_json;
		
		// get the extension
		auto ext = inpath.extension();
		if(ext == ".chigmod") {
			// load it as a module
			
			// TODO: implement
			
		} else if(ext == ".chigfn") {
			// load it as a function
			
			std::unique_ptr<GraphFunction> graphFunc;
			res = GraphFunction::fromJSON(c, read_json, &graphFunc);
			
			if(!res) continue;
			
			llvm::Function* func;
			res += graphFunc->compile(llmod.get(), &func);
			
		}

	}
	
	if(!res) {
		std::cerr << res.result_json.dump(2) << std::endl;
		return 1;
	}
	
}
