#include <fstream>
#include <iostream>
#include <memory>

#include <chi/Context.hpp>
#include <chi/GraphFunction.hpp>
#include <chi/GraphModule.hpp>
#include <chi/JsonDeserializer.hpp>
#include <chi/LangModule.hpp>
#include <chi/Result.hpp>
#include <chi/json.hpp>

#include <llvm/IR/Module.h>

using namespace chi;
using namespace nlohmann;

// returns -1 for failure, 1 for keep going  and 0 for success
int checkForErrors(Result res, const char* expectedErr) {
	if (!res) {
		if (res.result_json[0]["errorcode"] == expectedErr) {
			return 0;
		} else {
			std::cerr << "Expected error " << expectedErr << " but got "
			          << res.result_json[0]["errorcode"] << std::endl
			          << res << std::endl;
			return -1;
		}
	}

	return 1;
}

int main(int argc, char** argv) {
	const char* mode        = argv[1];
	const char* file        = argv[2];
	const char* expectedErr = argv[3];

	assert(boost::filesystem::is_regular_file(file) && "Pass a real file");

	std::ifstream ifile(file);
	std::string   str((std::istreambuf_iterator<char>(ifile)), std::istreambuf_iterator<char>());

	json newData;
	try {
		newData = json::parse(str);
	} catch (std::exception& e) {
		std::cerr << "Error parsing: " << e.what();
		return 1;
	}
	Context c;
	Result  res;

	if (strcmp(mode, "mod") == 0) {
		GraphModule* mod;
		res += jsonToGraphModule(c, newData, "main", &mod);

		int ret = checkForErrors(res, expectedErr);
		if (ret != 1) return ret;

		std::unique_ptr<llvm::Module> llmod = nullptr;
		res += c.compileModule(mod->fullName(), CompileSettings::Default, &llmod);

		ret = checkForErrors(res, expectedErr);
		if (ret != 1) return ret;

		return 1;

	} else if (strcmp(mode, "func") == 0) {
		auto mod = c.newGraphModule("main");
		mod->addDependency("lang");

		GraphFunction* func;
		res += createGraphFunctionDeclarationFromJson(*mod, newData, &func);

		int ret = checkForErrors(res, expectedErr);
		if (ret != 1) return ret;

		res += jsonToGraphFunction(*func, newData);

		ret = checkForErrors(res, expectedErr);
		if (ret != 1) return ret;

		// create module for the functions
		std::unique_ptr<llvm::Module> llmod;

		res += c.compileModule("main", CompileSettings::Default, &llmod);

		ret = checkForErrors(res, expectedErr);
		if (ret != 1) return ret;

		return 1;

	} else {
		std::cerr << "Unregnized mode: " << mode << std::endl;
		return 1;
	}
}
