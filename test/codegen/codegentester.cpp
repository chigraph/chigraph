#include <chig/Context.hpp>

#include <exec-stream.h>

#include <boost/filesystem.hpp>

#include <llvm/ADT/STLExtras.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/IR/Argument.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/Support/Casting.h>
#include <llvm/Support/ManagedStatic.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>


using namespace chig;
using namespace nlohmann;

namespace fs = boost::filesystem;

int main(int argc, char** argv) {
	
	
	if(argc != 2) {
		std::cerr << "Usage: codegen_tester JSONFILE" << std::endl;
		return 1;
	}
	
	fs::path JSONfile = argv[1];
	// load the file
	
	if(!fs::is_regular_file(JSONfile)) {
		std::cerr << JSONfile << " doesn't exist" << std::endl;
		return 1;
	}
	
	fs::ifstream jsonstream;
	
	json j;
	
	// parse the JSON
	try {
		jsonstream >> j;
	} catch(std::exception& e) {
		std::cerr << "Error parsing file " << JSONfile << " " << e.what() << std::endl;
		return 1;
	}
	
	std::string testdesc = j["testname"];
	
	std::string expectedcout = j["expectedstdout"];
	std::string expectedcerr = j["expectedstderr"];
	
	json chigmodule = j["module"];
	
	int returncode = j["exeectedret"];
	
	// this program is to be started where chigc is
	exec_stream_t chigcexe("./chigc", "-");
	chigcexe.in() << chigmodule;
	chigcexe.close_in();
	
	std::string outputir((std::istreambuf_iterator<char>(chigcexe.out())),
                 std::istreambuf_iterator<char>(chigcexe.out()));
	
	// create a llvm::Module
	llvm::SMDiagnostic err;
	llvm::LLVMContext c;
	llvm::MemoryBufferRef mem{llvm::StringRef(outputir), "chigc"};
	auto mod = llvm::parseIR(mem, err, c);
	
	// JIT the code!
	llvm::InitializeNativeTarget();
	
	auto EE = llvm::EngineBuilder(std::move(mod)).create();
	
	EE->runFunction(mod->getFunction("main"), {});

}

