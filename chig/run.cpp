#include <iostream>
#include <string>
#include <vector>

#include <chig/CModule.hpp>
#include <chig/Context.hpp>
#include <chig/GraphFunction.hpp>
#include <chig/JsonModule.hpp>
#include <chig/LangModule.hpp>
#include <chig/NodeType.hpp>
#include <chig/Result.hpp>
#include <chig/json.hpp>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/ExecutionEngine/Interpreter.h>
#include <llvm/ExecutionEngine/JITEventListener.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/ExecutionEngine/ObjectCache.h>
#include <llvm/ExecutionEngine/Orc/OrcRemoteTargetClient.h>
#include <llvm/ExecutionEngine/OrcMCJITReplacement.h>
#include <llvm/ExecutionEngine/SectionMemoryManager.h>

#include <llvm/CodeGen/LinkAllCodegenComponents.h>

#include <llvm/Support/TargetSelect.h>

namespace po = boost::program_options;
namespace fs = boost::filesystem;

using namespace chig;

int run(const std::vector<std::string>& opts)
{
	llvm::InitializeNativeTarget();
	llvm::InitializeNativeTargetAsmPrinter();
	llvm::InitializeNativeTargetAsmParser();

	po::options_description run_opts;
	run_opts.add_options()("input-file", po::value<std::string>(),
		"The input file, - for stdin. Should be a chig module")("workspace,w",
		po::value<std::string>(),
		"The workspace path. Leave blank to inferr from the working directory");

	po::positional_options_description pos;
	pos.add("input-file", 1);

	po::variables_map vm;
	po::store(po::command_line_parser(opts).options(run_opts).positional(pos).run(), vm);

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
			std::cerr << e.what() << std::endl;
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
	JsonModule* jmod = nullptr;
	c.addModuleFromJson("main", read_json, &jmod);

	if (!res) {
		std::cerr << res << std::endl;
		return 1;
	}

	std::unique_ptr<llvm::Module> llmod;
	res += c.compileModule(jmod->name(), &llmod);

	if (!res) {
		std::cerr << "Error compiling module: " << res << std::endl;
	}

	// run it!

	llvm::Function* entry = llmod->getFunction("main");

	if (entry == nullptr) {
		std::cerr << "No main function in module: " << std::endl;
		return 1;
	}

	llvm::EngineBuilder EEBuilder(std::move(llmod));

	EEBuilder.setEngineKind(llvm::EngineKind::JIT);
	EEBuilder.setVerifyModules(true);

	EEBuilder.setOptLevel(llvm::CodeGenOpt::Default);

	std::string errMsg;
	EEBuilder.setErrorStr(&errMsg);

	std::unique_ptr<llvm::ExecutionEngine> EE(EEBuilder.create());

	if (!EE) {
		std::cerr << errMsg << std::endl;
		return 1;
	}

	EE->runFunctionAsMain(entry, {}, nullptr);

	return 0;
}
