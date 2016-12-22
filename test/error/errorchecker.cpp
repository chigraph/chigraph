#include <fstream>
#include <iostream>
#include <memory>

#include <chig/Context.hpp>
#include <chig/GraphFunction.hpp>
#include <chig/JsonModule.hpp>
#include <chig/LangModule.hpp>
#include <chig/json.hpp>

using namespace chig;
using namespace nlohmann;

// returns -1 for failure, 1 for keep going  and 0 for success
int checkForErrors(Result res, const char* expectedErr)
{
	if (!res) {
		if (res.result_json[0]["errorcode"] == expectedErr) {
			return 0;
		} else {
			std::cerr << "Expected error " << expectedErr << " but got "
					  << res.result_json[0]["errorcode"] << std::endl;
			return -1;
		}
	}

	return 1;
}

int main(int argc, char** argv)
{
	const char* mode = argv[1];
	const char* file = argv[2];
	const char* expectedErr = argv[3];

	std::ifstream ifile(file);
	std::string str((std::istreambuf_iterator<char>(ifile)), std::istreambuf_iterator<char>());

	json newData;
	try {
		newData = json::parse(str);
	} catch (std::exception& e) {
		std::cerr << "Error parsing: " << e.what();
		return 1;
	}
	Context c;
	Result res;

	if (strcmp(mode, "mod") == 0) {
		auto mod = std::make_unique<JsonModule>(c, "main", newData, &res);
		std::string moduleName = mod->name();

		int ret = checkForErrors(res, expectedErr);
		if (ret != 1) return ret;

		res += mod->loadGraphs();

		ret = checkForErrors(res, expectedErr);
		if (ret != 1) return ret;

		c.addModule(std::move(mod));
		std::unique_ptr<llvm::Module> llmod = nullptr;
		res += c.compileModule(moduleName, &llmod);

		ret = checkForErrors(res, expectedErr);
		if (ret != 1) return ret;

		return 1;

	} else if (strcmp(mode, "func") == 0) {
		auto deps = std::vector<std::string>{"lang", "c"};
		auto uMod = std::make_unique<JsonModule>(
			c, "main", gsl::span<std::string>(deps.data(), deps.size()));
		auto modPtr = uMod.get();
		c.addModule(std::move(uMod));
		std::unique_ptr<GraphFunction> graphFunc;
		res = GraphFunction::fromJSON(*modPtr, newData, &graphFunc);

		int ret = checkForErrors(res, expectedErr);
		if (ret != 1) return ret;

		res += graphFunc->loadGraph();

		ret = checkForErrors(res, expectedErr);
		if (ret != 1) return ret;

		// create module for the functions
		std::unique_ptr<llvm::Module> llmod;

		res += c.compileModule("main", &llmod);

		ret = checkForErrors(res, expectedErr);
		if (ret != 1) return ret;

		return 1;

	} else {
		std::cerr << "Unregnized mode: " << mode << std::endl;
		return 1;
	}
}
