#include <iostream>
#include <string>
#include <vector>

#include <chi/Context.hpp>
#include <chi/GraphFunction.hpp>
#include <chi/GraphModule.hpp>
#include <chi/LangModule.hpp>
#include <chi/NodeType.hpp>
#include <chi/Result.hpp>
#include <chi/json.hpp>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/ExecutionEngine/Interpreter.h>
#include <llvm/ExecutionEngine/JITEventListener.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/ExecutionEngine/ObjectCache.h>
#include <llvm/ExecutionEngine/SectionMemoryManager.h>
#include <llvm/IR/Module.h>

#include <llvm/CodeGen/LinkAllCodegenComponents.h>

#include <llvm/Support/TargetSelect.h>

namespace po = boost::program_options;
namespace fs = boost::filesystem;

using namespace chi;

int run(const std::vector<std::string>& opts, const char* argv0) {
	po::options_description run_opts;
	run_opts.add_options()("input-file", po::value<std::string>(),
	                       "The input file, - for stdin. Should be a chig module")(
	    "subargs", po::value<std::vector<std::string>>(), "Arguments to call main with");

	po::positional_options_description pos;
	pos.add("input-file", 1).add("subargs", -1);

	po::variables_map vm;
	auto              parsed_opts =
	    po::command_line_parser(opts).options(run_opts).positional(pos).allow_unregistered().run();
	po::store(parsed_opts, vm);
	po::notify(vm);

	std::vector<std::string> command_opts =
	    po::collect_unrecognized(parsed_opts.options, po::include_positional);
	// add the name into it
	command_opts.insert(command_opts.begin(), argv0);

	if (vm.count("input-file") == 0) {
		std::cerr << "chig compile: error: no input files" << std::endl;
		return 1;
	}

	std::string infile = vm["input-file"].as<std::string>();

	Context c{fs::current_path()};

	// load module
	GraphModule* jmod = nullptr;

	Result res;

	if (infile == "-") {
		nlohmann::json read_json = {};
		std::cin >> read_json;
		res += c.addModuleFromJson("main", read_json, &jmod);

	} else {
		// make sure it's an actual file
		fs::path inpath = infile;
		// remove extension if the user added it
		inpath.replace_extension("");

		fs::path moduleName = fs::relative(fs::current_path(), c.workspacePath() / "src") / inpath;

		ChiModule* cMod;
		res += c.loadModule(moduleName.string(), LoadSettings::Default, &cMod);
		if (!res) {
			std::cerr << res.dump();
			return 1;
		}

		jmod = dynamic_cast<GraphModule*>(cMod);
	}

	if (!res) {
		std::cerr << res << std::endl;
		return 1;
	}

	std::unique_ptr<llvm::Module> llmod;
	res += c.compileModule(jmod->fullName(), true, &llmod);

	if (!res) { std::cerr << "Error compiling module: " << res << std::endl; return 1; }

	// run it!

	int ret;
	res += interpretLLVMIRAsMain(std::move(llmod), llvm::CodeGenOpt::Default, command_opts, nullptr,
	                             &ret);
	if (!res) {
		std::cerr << res << std::endl;
		return 1;
	}

	return ret;
}
