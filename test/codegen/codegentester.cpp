#include <chig/Context.hpp>

#include <exec-stream.h>

#include <boost/filesystem.hpp>

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

}

