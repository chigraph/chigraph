#include <iostream>
#include <string>
#include <vector>

#include <chig/CModule.hpp>
#include <chig/Context.hpp>
#include <chig/GraphFunction.hpp>
#include <chig/GraphModule.hpp>
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
#include <llvm/ExecutionEngine/SectionMemoryManager.h>

#include <llvm/CodeGen/LinkAllCodegenComponents.h>

#include <llvm/Support/TargetSelect.h>

namespace po = boost::program_options;
namespace fs = boost::filesystem;

using namespace chig;

int run(const std::vector<std::string>& opts) {
	llvm::InitializeNativeTarget();
	llvm::InitializeNativeTargetAsmPrinter();
	llvm::InitializeNativeTargetAsmParser();

	po::options_description run_opts;
	run_opts.add_options()("input-file", po::value<std::string>(),
	                       "The input file, - for stdin. Should be a chig module");

	po::positional_options_description pos;
	pos.add("input-file", 1);

	po::variables_map vm;
	po::store(po::command_line_parser(opts).options(run_opts).positional(pos).run(), vm);

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

		ChigModule* cMod;
		res += c.loadModule(moduleName.string(), &cMod);
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
	res += c.compileModule(jmod->fullName(), &llmod);

	if (!res) { std::cerr << "Error compiling module: " << res << std::endl; }

	// run it!

	llvm::Function* entry = llmod->getFunction("main");

	if (entry == nullptr) {
		std::cerr << "No main function in module: " << std::endl;

		std::string              mod;
		llvm::raw_string_ostream s{mod};
		llmod->print(s, nullptr);

		std::cerr << std::endl << mod << std::endl;
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

	return EE->runFunctionAsMain(entry, {}, nullptr);
}
