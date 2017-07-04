#include <chi/Context.hpp>
#include <chi/GraphModule.hpp>
#include <chi/JsonSerializer.hpp>
#include <chi/Support/Result.hpp>
#include <chi/Support/Subprocess.hpp>
#include <chi/Support/LibCLocator.hpp>

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

const char* exesuffix =
#ifdef WIN32
    "exe"
#else
    ""
#endif
    ;

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

	if (lhs["has_c_support"] != rhs["has_c_support"]) {
		return "has_c_support doesn't match: serialized: " + lhs["has_c_support"].dump(-1) +
		       " original: " + rhs["has_c_support"].dump(-1);
	}

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
		// compare nodes
		for (auto nodeIter = lgraph["nodes"].begin(); nodeIter != lgraph["nodes"].end();
		     ++nodeIter) {
			auto& lnode = nodeIter.value();
			auto& rnode = rgraph["nodes"][nodeIter.key()];
			if (std::abs(float(lnode["location"][0]) - float(rnode["location"][0])) >= .00001) {
				return "node with id: " + nodeIter.key() + " in graph #" + std::to_string(iter) +
				       " has a non-matching x location. Original: " +
				       std::to_string(float(lnode["location"][0])) +
				       " Serialized: " + std::to_string(float(rnode["location"][0]));
			}

			if (std::abs(float(lnode["location"][1]) - float(rnode["location"][1])) >= .00001) {
				return "node with id: " + nodeIter.key() + " in graph #" + std::to_string(iter) +
				       " has a non-matching y location. Original: " +
				       std::to_string(float(lnode["location"][1])) +
				       " Serialized: " + std::to_string(float(rnode["location"][1]));
			}

			if (lnode["data"].dump(-1) != rnode["data"].dump(-1)) {
				return "node with id: " + nodeIter.key() + " in graph #" + std::to_string(iter) +
				       " have non-matching data. Original: \n\n" + rnode["data"].dump(-1) +
				       "\n\nSerialized: \n\n" + lnode["data"].dump(-1);
			}

			if (lnode["type"] != rnode["type"]) {
				return "node with id: " + nodeIter.key() + " in graph #" + std::to_string(iter) +
				       " have non-matching types. Original: \n\n" + rnode["type"].dump(-1) +
				       "\n\nSerialized: \n\n" + lnode["type"].dump(-1);
			}
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
	
	// print the C include paths for debugging
	std::vector<fs::path> paths;
    stdCIncludePaths(paths);
    
    std::cout << "Standard C include paths: " << std::endl;
    for (const auto& p : paths) {
      std::cout << p.string() << std::endl;
    }
    std::cout << std::endl;

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

	std::cout << "Testing " << testdesc << " with expected stdout: \"" << expectedcout
	          << "\" and expected stderr \"" << expectedcerr << "\"" << std::endl;

	auto chiExePath = fs::current_path() / ("chi" + std::string(exesuffix));

	fs::current_path(moduleDir);

	std::cout << "Testing with chi compile | chi interpret...";
	std::cout.flush();

	Result res;

	// chig compile + lli
	{
		std::string generatedir, chigstderr;
		// go through chig compile
		{
			Subprocess chiexe{chiExePath};
			chiexe.setArguments({"compile", "-f", "main.chimod"});
			chiexe.attachToStdErr(
			    [&chigstderr](const char* data, size_t size) { chigstderr.append(data, size); });
			chiexe.attachToStdOut(
			    [&generatedir](const char* data, size_t size) { generatedir.append(data, size); });

			res += chiexe.start();
			if (!res) {
				std::cerr << res << std::endl;
				return 1;
			}

			// check stderr and return code
			if (chiexe.exitCode() != 0) {
				std::cerr << "Failed to generate module with chig compile: \n"
				          << chigstderr << std::endl;
				return 1;
			}
		}

		std::cout << "Compiled...";
		std::cout.flush();

		std::string llistdout, llistderr;

		{
			// now go through lli
			Subprocess interpretexe{chiExePath};
			interpretexe.setArguments({"interpret"});
			interpretexe.attachToStdErr(
			    [&llistderr](const char* data, size_t size) { llistderr.append(data, size); });
			interpretexe.attachToStdOut(
			    [&llistdout](const char* data, size_t size) { llistdout.append(data, size); });

			res += interpretexe.start();
			if (!res) {
				std::cerr << res << std::endl;
				return 1;
			}

			res += interpretexe.pushToStdIn(generatedir.data(), generatedir.size());
			if (!res) {
				std::cerr << res << std::endl;
				return 1;
			}

			res += interpretexe.closeStdIn();
			if (!res) {
				std::cerr << res << std::endl;
				return 1;
			}

			int retcodelli = interpretexe.exitCode();

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

		std::cout << "Suceeded." << std::endl;
	}

	std::cout << "Testing with chi run...";
	std::cout.flush();

	// chi run
	{
		std::string generatedstdout, generatedstderr;
		{
			Subprocess chiexe{chiExePath};
			chiexe.setArguments({"run", "main.chimod"});
			chiexe.attachToStdErr([&generatedstderr](const char* data, size_t size) {
				generatedstderr.append(data, size);
			});
			chiexe.attachToStdOut([&generatedstdout](const char* data, size_t size) {
				generatedstdout.append(data, size);
			});

			res += chiexe.start();
			if (!res) {
				std::cerr << res << std::endl;
				return 1;
			}

			int retcode = chiexe.exitCode();

			if (retcode != expectedreturncode) {
				std::cerr << "(chi run) Unexpected retcode: " << retcode << " expected was "
				          << expectedreturncode << std::endl
				          << "stdout: \"" << generatedstdout << "\"" << std::endl
				          << "stderr: \"" << generatedstderr << "\"" << std::endl;

				return 1;
			}

			if (generatedstdout != expectedcout) {
				std::cerr << "(chi run) Unexpected stdout: \"" << generatedstdout
				          << "\" expected was \"" << expectedcout << '\"' << std::endl
				          << "retcode: \"" << retcode << "\"" << std::endl
				          << "stderr: \"" << generatedstderr << "\"" << std::endl;

				return 1;
			}

			if (generatedstderr != expectedcerr) {
				std::cerr << "(chi run) Unexpected stderr: \"" << generatedstderr
				          << "\" expected was \"" << expectedcerr << '\"' << std::endl
				          << "retcode: \"" << retcode << "\"" << std::endl
				          << "stdout: \"" << generatedstdout << "\"" << std::endl;

				return 1;
			}
		}

		std::cout << "done." << std::endl;
	}

	std::cout << "testing with chi compile -c | chi interpret";
	std::cout.flush();
	{
		std::string generatedir, chigstderr;
		// go through chig compile
		{
			Subprocess chiexe{chiExePath};
			chiexe.setArguments({"compile", "-c", "-f", "main.chimod"});
			chiexe.attachToStdErr(
			    [&chigstderr](const char* data, size_t size) { chigstderr.append(data, size); });
			chiexe.attachToStdOut(
			    [&generatedir](const char* data, size_t size) { generatedir.append(data, size); });

			res += chiexe.start();
			if (!res) {
				std::cerr << res << std::endl;
				return 1;
			}

			// check stderr and return code
			if (chiexe.exitCode() != 0) {
				std::cerr << "Failed to generate module with chi compile: \n"
				          << chigstderr << std::endl;
				return 1;
			}
		}

		std::cout << "Compiled...";
		std::cout.flush();

		std::string llistdout, llistderr;

		{
			// now go through lli
			Subprocess interpretexe{chiExePath};
			interpretexe.setArguments({"interpret"});
			interpretexe.attachToStdErr(
			    [&llistderr](const char* data, size_t size) { llistderr.append(data, size); });
			interpretexe.attachToStdOut(
			    [&llistdout](const char* data, size_t size) { llistdout.append(data, size); });

			res += interpretexe.start();
			if (!res) {
				std::cerr << res << std::endl;
				return 1;
			}

			res += interpretexe.pushToStdIn(generatedir.data(), generatedir.size());
			if (!res) {
				std::cerr << res << std::endl;
				return 1;
			}

			res += interpretexe.closeStdIn();
			if (!res) {
				std::cerr << res << std::endl;
				return 1;
			}

			int retcodelli = interpretexe.exitCode();

			if (retcodelli != expectedreturncode) {
				std::cerr << "(lli bc) Unexpected retcode: " << retcodelli << " expected was "
				          << expectedreturncode << std::endl
				          << "stdout: \"" << llistdout << "\"" << std::endl
				          << "stderr: \"" << llistderr << "\"" << std::endl;
				return 1;
			}

			if (llistdout != expectedcout) {
				std::cerr << "(lli bc) Unexpected stdout: \"" << llistdout << "\" expected was \""
				          << expectedcout << "\"" << std::endl
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

		std::cout << "Suceeded." << std::endl;
	}

	std::cout << "Testing serialize+deserialize...";
	std::cout.flush();

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

		// load the module into memory
		auto         modfile = moduleDir / "main.chimod";
		fs::ifstream inmodfile(modfile);
		json         chimodule;
		inmodfile >> chimodule;

		std::string err = areJsonEqual(serializedmodule, chimodule);
		if (!err.empty()) {
			std::cerr << "Serialization and deserialization failed. error: " + err +
			                 "\n\n======ORIGINAL=======\n\n\n"
			          << chimodule.dump(-1) << "\n\n\n\n======SERIALIZED=====\n\n\n\n"
			          << serializedmodule.dump(-1) << std::endl;
			return 1;
		}
	}

	std::cout << "done." << std::endl;
}
