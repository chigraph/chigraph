#include <chig/Context.hpp>
#include <chig/Result.hpp>
#include <chig/JsonModule.hpp>
#include <chig/NodeType.hpp>
#include <chig/GraphFunction.hpp>
#include <chig/LangModule.hpp>
#include <chig/CModule.hpp>
#include <chig/Config.hpp>

#include <exec-stream.h>

#include <boost/filesystem.hpp>

#include <llvm/ADT/STLExtras.h>
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

std::string areArrayEqualUnordered(nlohmann::json lhs, nlohmann::json rhs) {
	std::vector<nlohmann::json> objects;
	
	for(auto& obj : lhs) {
		objects.push_back(obj);
	}
	
	for(auto& obj : rhs) {
		auto iter = std::find(objects.begin(), objects.end(), obj);
		
		if(iter == objects.end()) return "object: " + obj.dump(2) + " not in serialized";
		
		objects.erase(iter);
	}
	
	if(!objects.empty()) {
		std::string errstring = "objects in lhs that aren't in original: \n\n";
		for(auto obj : objects) {
			errstring += obj.dump(2);
			errstring += "\n\n";
		}
		return errstring;
	}
	
	return "";
}

std::string areJsonEqual(nlohmann::json lhs, nlohmann::json rhs) {
	
	std::string errstring;
	
	errstring = areArrayEqualUnordered(lhs["dependencies"], rhs["dependencies"]);
	if(!errstring.empty()) return "dependencies not equal: " + errstring;
	
	if(lhs["name"] != rhs["name"]) return "names not equal: serialized: " + lhs["name"].dump(-1) + " original: " + rhs["name"].dump(-1);
	
	auto& lgraphs = lhs["graphs"];
	auto& rgraphs = rhs["graphs"];
	
	if(lgraphs.size() != rgraphs.size()) return "different number of graphs";
	
	for(size_t iter = 0; iter != lgraphs.size(); ++iter) {
		
		auto& lgraph = lgraphs[0];
		auto& rgraph = rgraphs[0];
		
		errstring = areArrayEqualUnordered(lgraph["connections"], rgraph["connections"]);
		if(!errstring.empty()) return "connections not equal in graph #" + std::to_string(iter) + " " + errstring;
		
		if(lgraph["name"] != rgraph["name"]) return "graph name in graph #" + std::to_string(iter) + " not equal; serialized: " + lgraph["name"].dump(-1) + "  original: " + rgraph["name"].dump(-1);
		if(lgraph["nodes"] != rgraph["nodes"]) return "graph nodes in graph #" + std::to_string(iter) + " not equal; \nserialized: \n" + lgraph["nodes"].dump(-1) + "\n\noriginal:\n " + rgraph["nodes"].dump(-1);
		if(lgraph["type"] != rgraph["type"]) return "graph name in graph #" + std::to_string(iter) + "not equal; serialized: " + lgraph["type"].dump(-1) + "  original: " + rgraph["type"].dump(-1);
		if(lgraph["inputs"] != rgraph["inputs"]) return "graph inputs in graph #" + std::to_string(iter) + "not equal; serialized: " + lgraph["inputs"].dump(-1) + "  original: " + rgraph["inputs"].dump(-1);
		if(lgraph["outputs"] != rgraph["outputs"]) return "graph outputs in graph #" + std::to_string(iter) + "not equal; serialized: " + lgraph["outputs"].dump(-1) + "  original: " + rgraph["outputs"].dump(-1);
		
	}
	
	return "";
	
}

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
	
	const json chigmodule = j["module"];
	
	int expectedreturncode = j["expectedret"];
	
	// chig run
	{
	
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
			std::cerr << "(chig run) Unexpected retcode: " << retcode << " expected was " << expectedreturncode << std::endl << 
				"stdout: \"" << generatedstdout << "\"" << std::endl <<
				"stderr: \"" << generatedstderr << "\"" << std::endl;
				
			return 1;
		}
		
		if(generatedstdout != expectedcout) {
			std::cerr << "(chig run) Unexpected stdout: " << generatedstdout << " expected was " << expectedcout << std::endl << 
				"retcode: \"" << retcode << "\"" << std::endl <<
				"stderr: \"" << generatedstderr << "\"" << std::endl;
				
			return 1;
		}
		
		if(generatedstderr != expectedcerr) {
			std::cerr << "(chig run) Unexpected stderr: " << generatedstderr << " expected was " << expectedcerr << std::endl << 
				"retcode: \"" << retcode << "\"" << std::endl <<
				"stdout: \"" << generatedstdout << "\"" << std::endl;
				
			return 1;
		}
	
	}
	
	// serialize deserialize
	{
			
		Context c;
		c.addModule(std::make_unique<LangModule>(c));
		c.addModule(std::make_unique<CModule>(c));
		
		// test serialization and deserialization
		Result r;
		auto Udeserialized = std::make_unique<JsonModule>(chigmodule, c, &r);
        auto deserialized = Udeserialized.get();
        c.addModule(std::move(Udeserialized));
        deserialized->loadGraphs();

		nlohmann::json serializedmodule;
		r += deserialized->toJSON(&serializedmodule);

		if(!r) {
			std::cerr << "Error deserializing module: \n\n" << r.result_json.dump(2) << std::endl;
			return 1;
		}
		
		std::string err = areJsonEqual(serializedmodule, chigmodule);
		if(!err.empty())  {
			std::cerr << "Serialization and deserialization failed. error: " + err + "\n\noriginal: \n\n\n" << chigmodule.dump(-1) << "\n\n\n\n======SERIALIZED=====\n\n\n\n" << serializedmodule.dump(-1) << std::endl;
			return 1;
		}
	}

	// chig compile + lli
	{
		// go through chig compile
		exec_stream_t chigexe;
		chigexe.set_wait_timeout(exec_stream_t::s_out, 100000);
		chigexe.set_wait_timeout(exec_stream_t::s_err, 100000);
		
		std::vector<std::string> args2 = {"compile", "-"};
		chigexe.start("./chig", args2.begin(), args2.end());
		chigexe.in() << chigmodule;
		chigexe.close_in();
		
		// get the output streams
		std::string generatedir = std::string{std::istreambuf_iterator<char>(chigexe.out()),
			std::istreambuf_iterator<char>()};
		
		// now go through lli
		exec_stream_t lliexe;
		lliexe.set_wait_timeout(exec_stream_t::s_out, 100000);
		lliexe.set_wait_timeout(exec_stream_t::s_err, 100000);
		
		lliexe.start(CHIG_LLI_EXE, "");
		lliexe.in() << generatedir;
		lliexe.close_in();
		
		std::string llistdout = std::string{std::istreambuf_iterator<char>(lliexe.out()),
			std::istreambuf_iterator<char>()};
			
		std::string llistderr = std::string{std::istreambuf_iterator<char>(lliexe.err()),
			std::istreambuf_iterator<char>()};

		lliexe.close();
		int retcodelli = lliexe.exit_code();
			
		if(retcodelli != expectedreturncode) {
			std::cerr << "(lli ll) Unexpected retcode: " << retcodelli << " expected was " << expectedreturncode << std::endl << 
				"stdout: \"" << llistdout << "\"" << std::endl <<
				"stderr: \"" << llistderr << "\"" << std::endl;
				
			return 1;
		}
		
		if(llistdout != expectedcout) {
			std::cerr << "(lli ll) Unexpected stdout: " << llistdout << " expected was " << expectedcout << std::endl << 
				"retcode: \"" << retcodelli << "\"" << std::endl <<
				"stderr: \"" << llistderr << "\"" << std::endl;
				
			return 1;
		}
		
		if(llistderr != expectedcerr) {
			std::cerr << "(lli ll) Unexpected stderr: " << stderr << " expected was " << expectedcerr << std::endl << 
				"retcode: \"" << retcodelli << "\"" << std::endl <<
				"stdout: \"" << llistdout << "\"" << std::endl;
				
			return 1;
		}
	
	}
	
	// chig compile -tbc + lli
	{
		// go through chig compile
		exec_stream_t chigexe;
		chigexe.set_wait_timeout(exec_stream_t::s_out, 100000);
		chigexe.set_wait_timeout(exec_stream_t::s_err, 100000);
		
		std::vector<std::string> args2 = {"compile", "-", "-tbc"};
		chigexe.start("./chig", args2.begin(), args2.end());
		chigexe.in() << chigmodule;
		chigexe.close_in();
		
		// get the output streams
		std::string generatedbc = std::string{std::istreambuf_iterator<char>(chigexe.out()),
			std::istreambuf_iterator<char>()};
		
		// now go through lli
		exec_stream_t lliexe;
		lliexe.set_wait_timeout(exec_stream_t::s_out, 100000);
		lliexe.set_wait_timeout(exec_stream_t::s_err, 100000);
		
		lliexe.start(CHIG_LLI_EXE, "");
		lliexe.in() << generatedbc;
		lliexe.close_in();
		
		std::string llistdout = std::string{std::istreambuf_iterator<char>(lliexe.out()),
			std::istreambuf_iterator<char>()};
			
		std::string llistderr = std::string{std::istreambuf_iterator<char>(lliexe.err()),
			std::istreambuf_iterator<char>()};

		lliexe.close();
		int retcodelli = lliexe.exit_code();
			
		if(retcodelli != expectedreturncode) {
			std::cerr << "(lli bc) Unexpected retcode: " << retcodelli << " expected was " << expectedreturncode << std::endl << 
				"stdout: \"" << llistdout << "\"" << std::endl <<
				"stderr: \"" << llistderr << "\"" << std::endl;
				
			return 1;
		}
		
		if(llistdout != expectedcout) {
			std::cerr << "(lli bc) Unexpected stdout: " << llistdout << " expected was " << expectedcout << std::endl << 
				"retcode: \"" << retcodelli << "\"" << std::endl <<
				"stderr: \"" << llistderr << "\"" << std::endl;
				
			return 1;
		}
		
		if(llistderr != expectedcerr) {
			std::cerr << "(lli bc) Unexpected stderr: " << stderr << " expected was " << expectedcerr << std::endl << 
				"retcode: \"" << retcodelli << "\"" << std::endl <<
				"stdout: \"" << llistdout << "\"" << std::endl;
				
			return 1;
		}
	
	}
	
}

