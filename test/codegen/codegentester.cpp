#include <chi/Context.hpp>
#include <chi/GraphFunction.hpp>
#include <chi/GraphModule.hpp>
#include <chi/JsonSerializer.hpp>
#include <chi/LangModule.hpp>
#include <chi/NodeType.hpp>
#include <chi/Result.hpp>

#include <libexecstream/exec-stream.h>

#include <boost/filesystem.hpp>

#include <llvm/ADT/STLExtras.h>
#include <llvm/IR/Argument.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/Casting.h>
#include <llvm/Support/ManagedStatic.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>

using namespace chi;
using namespace nlohmann;

namespace fs = boost::filesystem;

#ifndef WIN32
const char* exesuffix = "";
#else
const char* exesuffix = ".exe";
#endif

std::string areArrayEqualUnordered(json lhs, json rhs) {
	std::vector<json> objects;

	for (auto& obj : lhs) { objects.push_back(obj); }

	for (auto& obj : rhs) {
		auto iter = std::find(objects.begin(), objects.end(), obj);

		if (iter == objects.end()) return "object: " + obj.dump(2) + " not in serialized";

		objects.erase(iter);
	}

	if (!objects.empty()) {
		std::string errstring = "objects in lhs that aren't in original: \n\n";
		for (auto obj : objects) {
			errstring += obj.dump(2);
			errstring += "\n\n";
		}
		return errstring;
	}

	return "";
}

std::string areJsonEqual(json lhs, json rhs) {
	std::string errstring;

	errstring = areArrayEqualUnordered(lhs["dependencies"], rhs["dependencies"]);
	if (!errstring.empty()) return "dependencies not equal: " + errstring;

	auto& lgraphs = lhs["graphs"];
	auto& rgraphs = rhs["graphs"];

	if (lgraphs.size() != rgraphs.size()) return "different number of graphs";

	for (size_t iter = 0; iter != lgraphs.size(); ++iter) {
		auto& lgraph = lgraphs[0];
		auto& rgraph = rgraphs[0];

		errstring = areArrayEqualUnordered(lgraph["connections"], rgraph["connections"]);
		if (!errstring.empty())
			return "connections not equal in graph #" + std::to_string(iter) + " " + errstring;

		if (lgraph["name"] != rgraph["name"]) {
			return "graph name in graph #" + std::to_string(iter) +
			       " not equal; serialized: " + lgraph["name"].dump(-1) +
			       "  original: " + rgraph["name"].dump(-1);
		}
		if (lgraph["description"] != rgraph["description"]) {
			return "graph description in graph #" + std::to_string(iter) +
			       " not equal; serialized: " + lgraph["description"].dump(-1) +
			       "  original: " + rgraph["description"].dump(-1);
		}
		if (lgraph["nodes"].dump(-1) != rgraph["nodes"].dump(-1)) {
			return "graph nodes in graph #" + std::to_string(iter) +
			       " not equal; \nserialized: \n" + lgraph["nodes"].dump(-1) + "\n\noriginal:\n" +
			       rgraph["nodes"].dump(-1);
		}
		if (lgraph["type"] != rgraph["type"]) {
			return "graph name in graph #" + std::to_string(iter) +
			       "not equal; serialized: " + lgraph["type"].dump(-1) +
			       "  original: " + rgraph["type"].dump(-1);
		}
		if (lgraph["data_inputs"] != rgraph["data_inputs"]) {
			return "graph data inputs in graph #" + std::to_string(iter) +
			       "not equal; serialized: " + lgraph["inputs"].dump(-1) +
			       "  original: " + rgraph["inputs"].dump(-1);
		}
		if (lgraph["data_outputs"] != rgraph["data_outputs"]) {
			return "graph data outputs in graph #" + std::to_string(iter) +
			       "not equal; serialized: " + lgraph["outputs"].dump(-1) +
			       "  original: " + rgraph["outputs"].dump(-1);
		}
		if (lgraph["exec_inputs"] != rgraph["exec_inputs"]) {
			return "graph exec inputs in graph #" + std::to_string(iter) +
			       "not equal; serialized: " + lgraph["inputs"].dump(-1) +
			       "  original: " + rgraph["inputs"].dump(-1);
		}
		if (lgraph["exec_outputs"] != rgraph["exec_outputs"]) {
			return "graph exec outputs in graph #" + std::to_string(iter) +
			       "not equal; serialized: " + lgraph["outputs"].dump(-1) +
			       "  original: " + rgraph["outputs"].dump(-1);
		}
		if (lgraph["local_variables"] != rgraph["local_variables"]) {
			return "graph local_variables in graph #" + std::to_string(iter) +
			       "not equal; serialized: " + lgraph["local_variables"].dump(-1) +
			       " original: " + rgraph["local_variables"].dump(-1);
		}
	}

	return "";
}

int main(int argc, char** argv) {
	if (argc != 2) {
		std::cerr << "Usage: codegen_tester <path to module>" << std::endl;
		return 1;
	}

	fs::path moduleDir = argv[1];
	if (!fs::is_directory(moduleDir)) {
		std::cerr << moduleDir << " is not a directory" << std::endl;
		return 1;
	}
	std::string testName = moduleDir.filename().string();

	fs::path JSONfile = moduleDir / (testName + ".json");
	// load the file

	if (!fs::is_regular_file(JSONfile)) {
		std::cerr << JSONfile << " doesn't exist" << std::endl;
		return 1;
	}

	fs::ifstream jsonstream{JSONfile};

	json j;

	// parse the JSON
	try {
		jsonstream >> j;
	} catch (std::exception& e) {
		std::cerr << "Error parsing file " << JSONfile << " " << e.what() << std::endl;
		return 1;
	}

	std::string testdesc = j["testname"];

	std::string expectedcout = j["expectedstdout"];
	std::string expectedcerr = j["expectedstderr"];

	int expectedreturncode = j["expectedret"];

	auto chigExePath = fs::current_path() / ("chi" + std::string(exesuffix));

	auto         modfile = moduleDir / "main.chimod";
	fs::ifstream inmodfile(modfile);
	json         chigmodule;
	inmodfile >> chigmodule;

	fs::current_path(moduleDir);

	// chig compile + lli
	{
		std::string generatedir, chigstderr;
		// go through chig compile
		exec_stream_t chigexe;
		chigexe.set_wait_timeout(exec_stream_t::s_out, 10000);
		chigexe.start(chigExePath.string(), "compile main.chimod");

		generatedir = std::string{std::istreambuf_iterator<char>(chigexe.out()),
		                          std::istreambuf_iterator<char>()};
		chigstderr = std::string{std::istreambuf_iterator<char>(chigexe.err()),
		                         std::istreambuf_iterator<char>()};

		chigexe.close();
		// check stderr and return code
		if (chigexe.exit_code() != 0) {
			std::cerr << "Failed to generate module with chig compile: \n"
			          << chigstderr << std::endl;
			return 1;
		}

		std::string llistdout, llistderr;

		// now go through lli
		exec_stream_t lliexe;
		lliexe.set_wait_timeout(exec_stream_t::s_out, 10000);
		lliexe.start(chigExePath.string(), "interpret");

		lliexe.in() << generatedir;
		lliexe.close_in();

		llistdout = std::string{std::istreambuf_iterator<char>(lliexe.out()),
		                        std::istreambuf_iterator<char>()};
		llistderr = std::string{std::istreambuf_iterator<char>(lliexe.err()),
		                        std::istreambuf_iterator<char>()};

		lliexe.close();

		int retcodelli = lliexe.exit_code();

		if (retcodelli != expectedreturncode) {
			std::cerr << "(lli ll) Unexpected retcode: " << retcodelli << " expected was "
			          << expectedreturncode << std::endl
			          << "stdout: \"" << llistdout << "\"" << std::endl
			          << "stderr: \"" << llistderr << "\"" << std::endl
			          << "generated IR" << std::endl
			          << generatedir << std::endl;

			return 1;
		}

		if (llistdout != expectedcout) {
			std::cerr << "(lli ll) Unexpected stdout: \"" << llistdout << "\" expected was \""
			          << expectedcout << "\"" << std::endl
			          << "retcode: \"" << retcodelli << "\"" << std::endl
			          << "stderr: \"" << llistderr << "\"" << std::endl
			          << "generated IR" << std::endl
			          << generatedir << std::endl;

			return 1;
		}

		if (llistderr != expectedcerr) {
			std::cerr << "(lli ll) Unexpected stderr: \"" << stderr << "\" expected was \""
			          << expectedcerr << '\"' << std::endl
			          << "retcode: \"" << retcodelli << "\"" << std::endl
			          << "stdout: \"" << llistdout << "\"" << std::endl
			          << "generated IR" << std::endl
			          << generatedir << std::endl;

			return 1;
		}
	}

	// chig run
	{
		std::string generatedstdout, generatedstderr;
		// go through chig compile
		exec_stream_t chigexe;
		chigexe.set_wait_timeout(exec_stream_t::s_out, 10000);
		chigexe.start(chigExePath.string(), "run main.chimod");

		generatedstdout = std::string{std::istreambuf_iterator<char>(chigexe.out()),
		                              std::istreambuf_iterator<char>()};
		generatedstderr = std::string{std::istreambuf_iterator<char>(chigexe.err()),
		                              std::istreambuf_iterator<char>()};

		chigexe.close();
		int retcode = chigexe.exit_code();

		if (retcode != expectedreturncode) {
			std::cerr << "(chig run) Unexpected retcode: " << retcode << " expected was "
			          << expectedreturncode << std::endl
			          << "stdout: \"" << generatedstdout << "\"" << std::endl
			          << "stderr: \"" << generatedstderr << "\"" << std::endl;

			return 1;
		}

		if (generatedstdout != expectedcout) {
			std::cerr << "(chig run) Unexpected stdout: \"" << generatedstdout
			          << "\" expected was \"" << expectedcout << '\"' << std::endl
			          << "retcode: \"" << retcode << "\"" << std::endl
			          << "stderr: \"" << generatedstderr << "\"" << std::endl;

			return 1;
		}

		if (generatedstderr != expectedcerr) {
			std::cerr << "(chig run) Unexpected stderr: \"" << generatedstderr
			          << "\" expected was \"" << expectedcerr << '\"' << std::endl
			          << "retcode: \"" << retcode << "\"" << std::endl
			          << "stdout: \"" << generatedstdout << "\"" << std::endl;

			return 1;
		}
	}

	// serialize deserialize
	{
		Result r;

		Context  c{moduleDir};
		fs::path fullName = fs::relative(moduleDir, c.workspacePath() / "src") / "main";

		// test serialization and deserialization
		GraphModule* deserialized;
		{
			ChiModule* cMod = nullptr;
			r += c.loadModule(fullName, LoadSettings::Default, &cMod);

			if (!r) {
				std::cerr << "Error loading module \n\n" << r << std::endl;
				return 1;
			}

			deserialized = dynamic_cast<GraphModule*>(cMod);
		}

		json serializedmodule = graphModuleToJson(*deserialized);

		if (!r) {
			std::cerr << "Error deserializing module: \n\n" << r << std::endl;
			return 1;
		}

		std::string err = areJsonEqual(serializedmodule, chigmodule);
		if (!err.empty()) {
			std::cerr << "Serialization and deserialization failed. error: " + err +
			                 "\n\n======ORIGINAL=======\n\n\n"
			          << chigmodule.dump(-1) << "\n\n\n\n======SERIALIZED=====\n\n\n\n"
			          << serializedmodule.dump(-1) << std::endl;
			return 1;
		}
	}

	// chig compile -tbc + lli
	{
		std::string generatedir, chigstderr;
		// go through chig compile
		exec_stream_t chigexe;
		chigexe.set_wait_timeout(exec_stream_t::s_out, 10000);
		chigexe.start(chigExePath.string(), "compile -tbc main.chimod");

		generatedir = std::string{std::istreambuf_iterator<char>(chigexe.out()),
		                          std::istreambuf_iterator<char>()};
		chigstderr = std::string{std::istreambuf_iterator<char>(chigexe.err()),
		                         std::istreambuf_iterator<char>()};

		chigexe.close();
		// check stderr and return code
		if (chigexe.exit_code() != 0) {
			std::cerr << "Failed to generate module with chig compile: \n"
			          << chigstderr << std::endl;
			return 1;
		}

		std::string llistdout, llistderr;

		// now go through lli
		exec_stream_t lliexe;
		lliexe.set_wait_timeout(exec_stream_t::s_out, 10000);
		lliexe.start(chigExePath.string(), "interpret");

		lliexe.in() << generatedir;
		lliexe.close_in();

		llistdout = std::string{std::istreambuf_iterator<char>(lliexe.out()),
		                        std::istreambuf_iterator<char>()};
		llistderr = std::string{std::istreambuf_iterator<char>(lliexe.err()),
		                        std::istreambuf_iterator<char>()};

		lliexe.close();
		int retcodelli = lliexe.exit_code();

		if (retcodelli != expectedreturncode) {
			std::cerr << "(lli bc) Unexpected retcode: " << retcodelli << " expected was "
			          << expectedreturncode << std::endl
			          << "stdout: \"" << llistdout << "\"" << std::endl
			          << "stderr: \"" << llistderr << "\"" << std::endl;

			return 1;
		}

		if (llistdout != expectedcout) {
			std::cerr << "(lli bc) Unexpected stdout: \"" << llistdout << "\" expected was \""
			          << expectedcout << '\"' << std::endl
			          << "retcode: \"" << retcodelli << "\"" << std::endl
			          << "stderr: \"" << llistderr << "\"" << std::endl;

			return 1;
		}

		if (llistderr != expectedcerr) {
			std::cerr << "(lli bc) Unexpected stderr: \"" << stderr << "\" expected was \""
			          << expectedcerr << '\"' << std::endl
			          << "retcode: \"" << retcodelli << "\"" << std::endl
			          << "stdout: \"" << llistdout << "\"" << std::endl;

			return 1;
		}
	}
}
