#include <iostream>
#include <string>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include <chi/Context.hpp>
#include <chi/GraphFunction.hpp>
#include <chi/GraphModule.hpp>
#include <chi/LLVMVersion.hpp>
#include <chi/LangModule.hpp>
#include <chi/NodeType.hpp>
#include <chi/Result.hpp>
#include <chi/json.hpp>

#if LLVM_VERSION_LESS_EQUAL(3, 9)
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
	compile_opts.add_options()("input-file", po::value<std::string>(), "Input file")(
	    "output,o", po::value<std::string>(), "Output file, - for stdout")(
	    "output-type,t", po::value<std::string>(),
	    "The output type, either bc or ll. If an output file is defined, then this can be "
	    "inferred")("directory,C", po::value<std::string>(),
	                "Directory to cd to before doing anything")
		("no-dependencies,D", "Don't link the dependencies into the module");

	po::positional_options_description pos;
	pos.add("input-file", 1);

	po::variables_map vm;
	po::store(po::command_line_parser(opts).options(compile_opts).positional(pos).run(), vm);

	if (vm.count("input-file") == 0) {
		std::cerr << "chi compile: error: no input files" << std::endl;
		return 1;
	}
	
	// cd first if it was specified
	if (vm.count("directory")) {
		fs::current_path(vm["directory"].as<std::string>());
	}

	fs::path infile = vm["input-file"].as<std::string>();

	Context        c{fs::current_path()};
	
	// add .chimod suffix if it doesn't have it
	if (infile.extension().empty()) {
		infile.replace_extension(".chimod");
	}
	
	// resolve the path---first see if it's relative to the current directory. if it's not, then
	// try to get it relative to 'src'
	infile = fs::absolute(infile, fs::current_path());
	if (!fs::is_regular_file(infile)) {
		infile = fs::absolute(infile, c.workspacePath() / "src");
		
		// if we still didn't find it, then error
		if (!fs::is_regular_file(infile)) {
			std::cerr << "chi compile: failed to find module: " << infile << std::endl;
			return 1;
		}
	}
	
	// get module name
	auto moduleName = fs::relative(infile, c.workspacePath() / "src").replace_extension("");

	Result res;

	// load it as a module
	ChiModule* chiModule;
	res += c.loadModule(moduleName, chi::LoadSettings::Default, &chiModule);

	if (!res) {
		std::cerr << res << std::endl;
		return 1;
	}

	std::unique_ptr<llvm::Module> llmod;
	res += c.compileModule(*chiModule, vm.count("no-dependencies") == 0, &llmod);

	if (!res) {
		std::cerr << "chi compile: Failed to compile module: " << std::endl << res << std::endl;
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

		std::error_code          ec;
		llvm::sys::fs::OpenFlags OpenFlags = llvm::sys::fs::F_None;
		if (outtype == "ll") { OpenFlags |= llvm::sys::fs::F_Text; }
		std::string errorString;  // only for LLVM 3.5-
		auto        outFile = std::make_unique<llvm::tool_output_file>
#if LLVM_VERSION_LESS_EQUAL(3, 5)
		    (outpath.string().c_str(), errorString, OpenFlags);
#else
		    (outpath.string(), ec, OpenFlags);
#endif
		if (outtype == "bc") {
			llvm::WriteBitcodeToFile(llmod.get(), outFile->os());
		} else if (outtype == "ll") {
			llmod->print(outFile->os(), nullptr);
		} else {
			std::cerr << "chi compile: Unrecognized output-type: " << outtype << std::endl;
			return 1;
		}
	}

	return 0;
}
