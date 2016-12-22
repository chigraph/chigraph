#include <iostream>
#include <string>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include <chig/CModule.hpp>
#include <chig/Context.hpp>
#include <chig/GraphFunction.hpp>
#include <chig/JsonModule.hpp>
#include <chig/LangModule.hpp>
#include <chig/NodeType.hpp>
#include <chig/Result.hpp>
#include <chig/json.hpp>

#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/raw_os_ostream.h>

using namespace chig;

namespace fs = boost::filesystem;
namespace po = boost::program_options;

int compile(const std::vector<std::string>& opts)
{
	po::options_description compile_opts("compile options");
	compile_opts.add_options()("input-file", po::value<std::string>(), "Input file, - for stdin")(
		"output,o", po::value<std::string>(), "Output file, - for stdout")("output-type,t",
		po::value<std::string>(),
		"The output type, either bc or ll. If an output file is defined, then this can be "
		"inferred")("workspace,w", po::value<std::string>(),
		"The workspace path. Leave blank to inferr from the working directory");

	po::positional_options_description pos;
	pos.add("input-file", 1);

	po::variables_map vm;
	po::store(po::command_line_parser(opts).options(compile_opts).positional(pos).run(), vm);

	if (vm.count("input-file") == 0) {
		std::cerr << "chig compile: error: no input files" << std::endl;
		return 1;
	}

	std::string infile = vm["input-file"].as<std::string>();

	nlohmann::json read_json = {};

	if (infile == "-") {
		std::cin >> read_json;
	} else {
		// make sure it's an actual file
		fs::path inpath = infile;
		if (!fs::is_regular_file(inpath)) {
			std::cerr << "error: Cannot open input file " << inpath;
			return 1;
		}

		fs::ifstream stream(inpath);

		try {
			stream >> read_json;
		} catch (std::exception& e) {
			std::cerr << "Error reading JSON: " << e.what() << std::endl;
			return 1;
		}
	}

	Result res;

	fs::path workspacePath;
	if (vm.count("workspace") != 0) {
		workspacePath = vm["workspace"].as<std::string>();
	} else {
		workspacePath = fs::current_path();
	}

	Context c{workspacePath};
	// load it as a module
	JsonModule* cmodule;
	res += c.addModuleFromJson("main", read_json, &cmodule);

	if (!res) {
		std::cerr << res << std::endl;
		return 1;
	}

	std::unique_ptr<llvm::Module> llmod;
	res += c.compileModule(cmodule->name(), &llmod);

	if (!res) {
		std::cerr << res << std::endl;
		return 1;
	}

	fs::path outpath;
	if (vm.count("output") != 0) {
		outpath = vm["output"].as<std::string>();
	} else {
		outpath = "-";
	}

	// create output
	{
		std::error_code ec;
		auto lloutstream =
			outpath.string() == "-"
				? std::unique_ptr<llvm::raw_ostream>(
					  std::make_unique<llvm::raw_os_ostream>(std::cout))
				: std::unique_ptr<llvm::raw_ostream>(std::make_unique<llvm::raw_fd_ostream>(
					  outpath.string(), ec, llvm::sys::fs::F_None));

		std::string outtype;
		if (vm.count("output-type") != 0) {
			outtype = vm["output-type"].as<std::string>();

		} else {
			// Then infer from extension or ll if cout is being used
			outtype = outpath.string() == "-" ? "ll" : outpath.extension().string().substr(1);
		}

		if (outtype == "bc") {
			llvm::WriteBitcodeToFile(llmod.get(), *lloutstream);
		} else if (outtype == "ll") {
			llmod->print(*lloutstream, nullptr);
		} else {
			std::cerr << "Unrecognized output-type: " << outtype << std::endl;
			return 1;
		}
	}

	return 0;
}
