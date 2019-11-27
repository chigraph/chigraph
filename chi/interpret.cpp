#include <llvm-c/ExecutionEngine.h>
#include <llvm-c/IRReader.h>
#include <llvm-c/Linker.h>

#include <boost/program_options.hpp>
#include <chi/Context.hpp>
#include <chi/Owned.hpp>
#include <chi/Support/Result.hpp>
#include <deque>
#include <iostream>
#include <string>
#include <vector>

namespace po = boost::program_options;
using namespace chi;

int interpret(const std::vector<std::string>& opts, const char* argv0) {
	po::options_description interpret_ops("interpret options");
	interpret_ops.add_options()(
	    "input-file,i",
	    po::value<std::vector<std::string>>()->default_value(std::vector<std::string>({"-"}), "-"),
	    "Input file, - for stdin")("optimization,O", po::value<int>()->default_value(2),
	                               "Optimization value, either 0, 1, 2, or 3")(
	    "function,f", po::value<std::string>()->default_value("main"), "The function to run")(
	    "subargs", po::value<std::vector<std::string>>(), "arguments for command");

	po::positional_options_description pos;
	pos.add("subargs", -1);

	po::variables_map vm;

	auto parsed_options = po::command_line_parser(opts)
	                          .options(interpret_ops)
	                          .positional(pos)
	                          .allow_unregistered()
	                          .run();
	po::store(parsed_options, vm);
	po::notify(vm);

	std::vector<std::string> command_opts =
	    po::collect_unrecognized(parsed_options.options, po::include_positional);
	// add the name into it
	command_opts.insert(command_opts.begin(), argv0);

	auto infiles = vm["input-file"].as<std::vector<std::string>>();

	// get opt value
	LLVMCodeGenOptLevel optLevel;
	switch (vm["optimization"].as<int>()) {
	case 0: optLevel = LLVMCodeGenLevelNone; break;
	case 1: optLevel = LLVMCodeGenLevelLess; break;
	case 2: optLevel = LLVMCodeGenLevelDefault; break;
	case 3: optLevel = LLVMCodeGenLevelAggressive; break;
	default:
		std::cerr << "Unrecognized optimization level: " << vm["optimization"].as<int>()
		          << std::endl;
		return 1;
	}

	Context ctx;

	// load the modules
	std::deque<OwnedLLVMModule> mods;
	for (const auto& file : infiles) {
		OwnedLLVMMemoryBuffer buffer;
		if (file == "-") {
			OwnedMessage message;
			if (LLVMCreateMemoryBufferWithSTDIN(&*buffer, &*message)) {
				std::cerr << "Failed to create a memory buffer from stdin: " << *message
				          << std::endl;
				return 1;
			}
		} else {
			OwnedMessage message;
			if (LLVMCreateMemoryBufferWithContentsOfFile(file.c_str(), &*buffer, &*message)) {
				std::cerr << "Failed to open input file \"" << file << "\": " << message
				          << std::endl;
				return 1;
			}
		}

		OwnedLLVMModule mod;
		OwnedMessage    message;
		// LLVMParseIRInContext takes ownership, interestingly enough.
		// https://llvm.org/doxygen/IRReader_8cpp_source.html#l00115
		if (LLVMParseIRInContext(ctx.llvmContext(), buffer.take_ownership(), &*mod, &*message)) {
			std::cerr << "Failed to parse IR from \"" << file << "\": " << message << std::endl;
			return 1;
		}
		mods.push_back(std::move(mod));
	}

	// link them all together
	OwnedLLVMModule realMod = std::move(mods[0]);
	mods.pop_front();

	while (!mods.empty()) {
		if (LLVMLinkModules2(*realMod, mods[0].take_ownership())) {
			std::cerr << "Failed to link modules." << std::endl;
			return 1;
		}
		mods.pop_front();
	}

	// run it
	auto func = LLVMGetNamedFunction(*realMod, vm["function"].as<std::string>().c_str());
	if (func == nullptr) {
		std::cerr << "Failed to find function in module: " << vm["function"].as<std::string>()
		          << std::endl;

		return 1;
	}

	int ret;

	auto res = interpretLLVMIRAsMain(std::move(realMod), optLevel, command_opts, func, &ret);
	if (!res) {
		std::cerr << "Faied to run module: " << std::endl << res << std::endl;
		return 1;
	}

	return ret;
}
