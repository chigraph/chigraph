#include <iostream>
#include <string>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include <chi/Context.hpp>
#include <chi/GraphFunction.hpp>
#include <chi/GraphModule.hpp>
#include <chi/LangModule.hpp>
#include <chi/NodeType.hpp>
#include <chi/Result.hpp>
#include <chi/json.hpp>

#if LLVM_VERSION_MAJOR <= 3
#include <llvm/Bitcode/ReaderWriter.h>
#else
#include <llvm/Bitcode/BitcodeReader.h>
#include <llvm/Bitcode/BitcodeWriter.h>
#endif

#include <llvm/IR/Module.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/ToolOutputFile.h>
#include <llvm/Support/raw_os_ostream.h>

using namespace chi;

namespace fs = boost::filesystem;
namespace po = boost::program_options;

int compile(const std::vector<std::string>& opts) {
	po::options_description compile_opts("compile options");
	compile_opts.add_options()("input-file", po::value<std::string>(), "Input file, - for stdin")(
	    "output,o", po::value<std::string>(), "Output file, - for stdout")(
	    "output-type,t", po::value<std::string>(),
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

	fs::path workspacePath;
	if (vm.count("workspace") != 0) {
		workspacePath = vm["workspace"].as<std::string>();
	} else {
		workspacePath = fs::current_path();
	}

	Context        c{workspacePath};
	nlohmann::json read_json = {};

	std::string moduleName;
	if (infile == "-") {
		std::cin >> read_json;
		moduleName = "main";
	} else {
		// make sure it's an actual file
		fs::path inpath = infile;
		if (!fs::is_regular_file(inpath)) {
			std::cerr << "error: Cannot open input file " << inpath;
			return 1;
		}
		moduleName = fs::relative(inpath, c.workspacePath() / "src").replace_extension("").string();

		fs::ifstream stream(inpath);

		try {
			stream >> read_json;
		} catch (std::exception& e) {
			std::cerr << "Error reading JSON: " << e.what() << std::endl;
			return 1;
		}
	}

	Result res;

	// load it as a module
	GraphModule* cmodule;
	res += c.addModuleFromJson(moduleName, read_json, &cmodule);

	if (!res) {
		std::cerr << res << std::endl;
		return 1;
	}

	std::unique_ptr<llvm::Module> llmod;
	res += c.compileModule(cmodule->fullName(), &llmod);

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
		std::string outtype;
		if (vm.count("output-type") != 0) {
			outtype = vm["output-type"].as<std::string>();

		} else {
			// Then infer from extension or ll if cout is being used
			outtype = outpath.string() == "-" ? "ll" : outpath.extension().string().substr(1);
		}

		std::error_code ec;
		llvm::sys::fs::OpenFlags OpenFlags = llvm::sys::fs::F_None;
		if (outtype == "ll") {
			OpenFlags |= llvm::sys::fs::F_Text;
		}
		std::string errorString; // only for LLVM 3.5-
		auto outFile = std::make_unique<llvm::tool_output_file>
#if LLVM_VERSION_MAJOR <= 3 && LLVM_VERSION_MINOR <= 5
			(output.string().c_str(), errorString, OpenFlags);
#else
			(outpath.string(), ec, OpenFlags);
#endif
		if (outtype == "bc") {
			llvm::WriteBitcodeToFile(llmod.get(), outFile->os());
		} else if (outtype == "ll") {
			llmod->print(outFile->os(), nullptr);
		} else {
			std::cerr << "Unrecognized output-type: " << outtype << std::endl;
			return 1;
		}
	}

	return 0;
}
