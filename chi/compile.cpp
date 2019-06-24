#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <boost/program_options.hpp>

#include <chi/Context.hpp>
#include <chi/GraphFunction.hpp>
#include <chi/GraphModule.hpp>
#include <chi/LangModule.hpp>
#include <chi/NodeType.hpp>
#include <chi/Support/Result.hpp>
#include <chi/Support/json.hpp>

#include <llvm-c/BitWriter.h>
#include <llvm-c/Transforms/PassManagerBuilder.h>

using namespace chi;

namespace fs = std::filesystem;
namespace po = boost::program_options;

int compile(const std::vector<std::string>& opts) {
	po::options_description compile_opts("chi compile");

	// clang-format off
	compile_opts.add_options()
		("input-file", po::value<std::string>(), "Input file")
		("output,o", po::value<std::string>()->default_value("-"), "Output file, - for stdout (the default)")
		(",c", "Output a binary file (llvm bitcode)")
		(",S", "Output a textual file (llvm assembly)")
		("no-dependencies,D", "Don't link the dependencies into the module")
		("fresh,f", "Don't use the cache")
		("machine-readable,m", "Create machine readable error messages (in JSON)")
		("no-debug,n", "Strip debug information from the module")
		("help,h", "Show this help page")
		("optimization,O", po::value<int>()->default_value(2), "The optimization level. Either 0, 1, 2, or 3")
		;
	// clang-format on

	po::positional_options_description pos;
	pos.add("input-file", 1);

	po::variables_map vm;
	po::store(po::command_line_parser(opts).options(compile_opts).positional(pos).run(), vm);

	if (vm.count("help") != 0) {
		std::cerr << compile_opts << std::endl;
		return 0;
	}

	if (vm.count("input-file") == 0) {
		std::cerr << "chi compile: error: no input files. Use chi compile --help for usage."
		          << std::endl;
		return 1;
	}

	if (vm.count("help") != 0) {
		std::cerr << compile_opts << std::endl;
		return 0;
	}

	fs::path infile = vm["input-file"].as<std::string>();

	Context c{fs::current_path()};

	// add .chimod suffix if it doesn't have it
	if (infile.extension().empty()) { infile.replace_extension(".chimod"); }

	// resolve the path---first see if it's relative to the current directory. if it's not, then
	// try to get it relative to 'src'
	auto infileRelToPwd = fs::absolute(infile);
	if (!fs::is_regular_file(infileRelToPwd)) {
		auto infileRelToSrc = fs::absolute(c.workspacePath() / "src" / infile);

		// if we still didn't find it, then error
		if (!fs::is_regular_file(infileRelToSrc)) {
			std::cerr << "chi compile: failed to find module: " << infile << std::endl;
			return 1;
		} else {
			infile = infileRelToSrc;
		}
	} else {
		infile = infileRelToPwd;
	}

	// get module name
	auto moduleName = fs::relative(infile, c.workspacePath() / "src").replace_extension("");

	Result res;

	// load it as a module
	ChiModule* chiModule;
	res += c.loadModule(moduleName, &chiModule);

	if (!res) {
		std::cerr << res << std::endl;
		return 1;
	}

	// make settings
	Flags<CompileSettings> settings;
	if (vm.count("no-dependencies") == 0) { settings |= CompileSettings::LinkDependencies; }
	if (vm.count("fresh") == 0) { settings |= CompileSettings::UseCache; }

	OwnedLLVMModule llmod;
	res += c.compileModule(*chiModule, settings, &llmod);

	if (!res) {
		if (vm.count("machine-readable") == 0) {
			std::cerr << "chi compile: Failed to compile module: " << std::endl << res << std::endl;
		} else {
			std::cerr << res.result_json.dump(2) << std::endl;
		}
		return 1;
	}

	// strip debug if specified
	if (vm.count("no-debug") != 0) { LLVMStripModuleDebugInfo(*llmod); }

	// optimize

	// get the level
	auto levelInt = vm["optimization"].as<int>();

	// vaidate it
	if (levelInt < 0 || levelInt > 3) {
		std::cerr << "Unrecognized optimization level: " << levelInt << std::endl;
		return 1;
	}

	auto passBuilder = OwnedLLVMPassManagerBuilder(LLVMPassManagerBuilderCreate());
	LLVMPassManagerBuilderSetOptLevel(*passBuilder, levelInt);

	// add inliner
	if (levelInt > 1) {
		LLVMPassManagerBuilderUseInlinerWithThreshold(*passBuilder, 250);  // 250 is default for -O3
	}

	auto fpm = OwnedLLVMPassManager(LLVMCreateFunctionPassManagerForModule(*llmod));
	LLVMPassManagerBuilderPopulateFunctionPassManager(*passBuilder, *fpm);

	auto mpm = OwnedLLVMPassManager(LLVMCreatePassManager());
	LLVMPassManagerBuilderPopulateModulePassManager(*passBuilder, *mpm);

	// run function passes
	for (auto func = LLVMGetFirstFunction(*llmod); func != nullptr;
	     func      = LLVMGetNextFunction(func)) {
		LLVMRunFunctionPassManager(*fpm, func);
	}

	LLVMRunPassManager(*mpm, *llmod);

	// get outpath
	fs::path outpath = vm["output"].as<std::string>();

	// get output type
	bool binaryOutput = false;

	// first see if we can deduce from output file
	if (outpath != "-") {
		auto ext = outpath.extension();
		if (ext == ".ll") { binaryOutput = false; }
		if (ext == ".bc") { binaryOutput = true; }
	}

	// then see if options were applied--these take precedence

	// first make sure they weren't both specified
	if (vm.count("-S") != 0 && vm.count("c") != 0) {
		std::cerr << "chi compile: cannot specify both -S and -c, please only specify one"
		          << std::endl;
		return 1;
	}

	if (vm.count("-S") != 0) { binaryOutput = false; }
	if (vm.count("-c") != 0) { binaryOutput = true; }

	auto writeOutput = [&](std::ostream& outputStream) {
		if (binaryOutput) {
			auto buffer = OwnedLLVMMemoryBuffer(LLVMWriteBitcodeToMemoryBuffer(*llmod));

			outputStream.write(LLVMGetBufferStart(*buffer), LLVMGetBufferSize(*buffer));
			outputStream.flush();
		} else {
			auto module = OwnedMessage(LLVMPrintModuleToString(*llmod));

			outputStream << module;
			outputStream.flush();
		}
	};

	if (outpath == "-") {
		writeOutput(std::cout);
	} else {
		std::ofstream file(outpath, binaryOutput ? std::ios_base::out | std::ios_base::binary
		                                         : std::ios_base::out);
		writeOutput(file);
	}

	return 0;
}
