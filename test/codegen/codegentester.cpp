#include <chig/Context.hpp>
#include <chig/Result.hpp>
#include <chig/JsonModule.hpp>
#include <chig/NodeType.hpp>
#include <chig/GraphFunction.hpp>

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

#include <chig/json.hpp>

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
	
	fs::ifstream jsonstream{JSONfile};
	
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
	
	int expectedreturncode = j["expectedret"];
	
	// this program is to be started where chigc is
	exec_stream_t chigexe;
	chigexe.set_wait_timeout(exec_stream_t::s_out, 100000);
	chigexe.set_wait_timeout(exec_stream_t::s_err, 100000);
	
	std::vector<std::string> args = {"run", "-"};
	chigexe.start("./chig", args.begin(), args.end());
	chigexe.in() << chigmodule;
	chigexe.close_in();
	
	// get the output streams
	std::string generatedstdout = std::string{std::istreambuf_iterator<char>(chigexe.out()),
		std::istreambuf_iterator<char>()};
	
	std::string generatedstderr = std::string{std::istreambuf_iterator<char>(chigexe.err()),
		std::istreambuf_iterator<char>()};
	
	chigexe.close();
	int retcode = chigexe.exit_code();
	
	if(retcode != expectedreturncode) {
		std::cerr << "Unexpected retcode: " << retcode << " expected was " << expectedreturncode << std::endl << 
			"stdout: \"" << stdout << "\"" << std::endl <<
			"stderr: \"" << stderr << "\"" << std::endl;
			
		return 1;
	}
	
	if(generatedstdout != expectedcout) {
		std::cerr << "Unexpected stdout: " << stdout << " expected was " << expectedcout << std::endl << 
			"retcode: \"" << retcode << "\"" << std::endl <<
			"stderr: \"" << stderr << "\"" << std::endl;
			
		return 1;
	}
	
	if(generatedstderr != expectedcerr) {
		std::cerr << "Unexpected stderr: " << stderr << " expected was " << expectedcerr << std::endl << 
			"retcode: \"" << retcode << "\"" << std::endl <<
			"stdout: \"" << stdout << "\"" << std::endl;
			
		return 1;
	}
	
	Context c;
	
	// test serialization and deserialization
	Result r;
	JsonModule deserialized(chigmodule, c, &r);

	nlohmann::json serializedmodule;
	r += deserialized.toJSON(&serializedmodule);

	if(serializedmodule != chigmodule)  {
		std::cerr << "Serialization and deserialization failed. \noriginal: " << chigmodule.dump(2) << "\n\nserialized: " << serializedmodule.dump(2) << std::endl;
	}


}

