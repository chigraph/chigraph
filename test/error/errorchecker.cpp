#include <iostream>
#include <fstream>
#include <memory>

#include <chig/GraphFunction.hpp>
#include <chig/Context.hpp>



#include <chig/json.hpp>

using namespace chig;
using namespace nlohmann;

int main(int argc, char** argv) {
	
	const char* file = argv[1];
	const char* expectedErr = argv[2];

	
	
	std::ifstream ifile(file);
	std::string str((std::istreambuf_iterator<char>(ifile)),
                 std::istreambuf_iterator<char>());
	
	auto newData = json::parse(str);
	
	Context c;
	Result res;
	
	std::unique_ptr<GraphFunction> graphFunc;
	res = GraphFunction::fromJSON(c, newData, &graphFunc);
	
	if(!res) {
		if(res.result_json[0]["errorcode"] == expectedErr) {
			return 0;
		} else {
			std::cerr << "Expected error " << expectedErr << " but got " << res.result_json[0]["errorcode"] << std::endl;
			return 1;
		}
	}
	
	// create module for the functions
	auto llmod = std::make_unique<llvm::Module>("main", c.context);
	
	
	llvm::Function* func;
	res += graphFunc->compile(llmod.get(), &func);
	
	
	if(!res) {
		if(res.result_json[0]["errorcode"] == expectedErr) {
			return 0;
		} else {
			std::cerr << "Expected error " << expectedErr << " but got " << res.result_json[0]["errorcode"] << std::endl;
			return 1;
		}
	}
	
	return 1;
} 
