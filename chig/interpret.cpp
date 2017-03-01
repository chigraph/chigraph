#include <chig/Context.hpp>
#include <chig/Result.hpp>
#include <chig/ChigModule.hpp>

#include <llvm/IR/Module.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/raw_os_ostream.h>
#include <llvm/Linker/Linker.h>


#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/ExecutionEngine/Interpreter.h>
#include <llvm/ExecutionEngine/JITEventListener.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/ExecutionEngine/ObjectCache.h>
#include <llvm/ExecutionEngine/SectionMemoryManager.h>

#include <vector>
#include <string>

#include <boost/program_options.hpp>

namespace po = boost::program_options;

int interpret(const std::vector<std::string>& opts) {
	po::options_description interpret_ops("interpret options");
	interpret_ops.add_options()("input-file", po::value<std::vector<std::string>>()->default_value(std::vector<std::string>({"-"}), "-"), "Input file, - for stdin")("optimization,O", po::value<int>()->default_value(2), "Optimization value, either 0, 1, 2, or 3")("function,f", po::value<std::string>()->default_value("main"), "The function to run");
	
	po::positional_options_description pos;
	pos.add("input-file", -1);
	
	
	po::variables_map vm;
	po::store(po::command_line_parser(opts).options(interpret_ops).positional(pos).run(), vm);

	auto infiles = vm["input-file"].as<std::vector<std::string>>();
	
	// get opt value
	llvm::CodeGenOpt::Level optLevel;
	switch(vm["optimization"].as<int>()) {
		case 0: optLevel = llvm::CodeGenOpt::None; break;
		case 1: optLevel = llvm::CodeGenOpt::Less; break;
		case 2: optLevel = llvm::CodeGenOpt::Default; break;
		case 3: optLevel = llvm::CodeGenOpt::Aggressive; break;
		default: 
			std::cerr << "Unrecognized optimization level: " << vm["optimization"].as<int>() << std::endl;
			return 1;
	}
	
	chig::Context ctx;
	
	// load the modules
	std::deque<std::unique_ptr<llvm::Module>> mods;
	for (const auto& file : infiles) {
		llvm::SMDiagnostic err;
		
		// this recognizes stdin correctly
		auto mod = llvm::parseIRFile(file, err, ctx.llvmContext());
		
		if (mod == nullptr) {
			llvm::raw_os_ostream errstream(std::cerr);
			std::cerr << "Failed to read IR: " << std::endl;
			err.print("chig", errstream);
			std::cerr << std::endl;
			
			return 1;
		}
		mods.push_back(std::move(mod));
	}
	
	// link them all together
	std::unique_ptr<llvm::Module> realMod = std::move(mods[0]);
	mods.pop_front();
	
	while(!mods.empty()) {
#if LLVM_VERSION_MAJOR <= 3 && LLVM_VERSION_MINOR <= 7
		llvm::Linker::LinkModules(realMod.get(), mods[0].get());
#else
		llvm::Linker::linkModules(*realMod, std::move(mods[0]));
#endif
		mods.pop_front();
	}
	
	// run it
	llvm::Function* func = realMod->getFunction(vm["function"].as<std::string>());
	
	if (func == nullptr) {
		std::cerr << "Failed to find function in module: " << vm["optimization"].as<int>() << std::endl;
		
		return 1;
	}
	
	llvm::GenericValue ret;
	auto res = chig::interpretLLVMIR(std::move(realMod), optLevel, {}, &ret, func);
	if (!res) {
		std::cerr << "Faied to run module: " << std::endl << res << std::endl;
		return 1;
	}
	
	return ret.IntVal.getZExtValue();
}
