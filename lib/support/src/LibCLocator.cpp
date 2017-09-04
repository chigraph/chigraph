
#include "chi/Support/LibCLocator.hpp"

// This requires 3 separate implementations--one for windows, one for linux, and one for macOS.

namespace fs = boost::filesystem;

#if defined __linux__ || defined __APPLE__

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <cstdlib>

#include "chi/Support/Result.hpp"
#include "chi/Support/Subprocess.hpp"

namespace chi {

// The linux implementation finds gcc and runs what's basically gcc -xc -E -v - < /dev/null which
// prints out the search paths.

Result stdCIncludePaths(std::vector<boost::filesystem::path>& toFill) {
	Result res;

	// first see if CHI_STDC_INCLUDE_PATH is defined in the environment
	auto envIncPath = std::getenv("CHI_STDC_INCLUDE_PATH");
	if (envIncPath != nullptr) {
		std::string envIncPathStr = envIncPath;

		std::vector<std::string> parsedOut;
		boost::algorithm::split(parsedOut, envIncPathStr, boost::algorithm::is_any_of(":"));

		std::transform(parsedOut.begin(), parsedOut.end(), std::back_inserter(toFill),
		               [](auto p) { return fs::path{p}; });
		return res;
	}

	// find gcc
	std::string              colonSeparatedPath = std::getenv("PATH");
	std::vector<std::string> paths;
	boost::algorithm::split(paths, colonSeparatedPath, boost::algorithm::is_any_of(":"));

	fs::path gccPath;
	for (const auto& possiblePath : paths) {
		auto pathToCheck = fs::path{possiblePath} / "gcc";

		if (fs::is_regular_file(pathToCheck) || fs::is_symlink(pathToCheck)) {
			gccPath = pathToCheck;
			break;
		}
	}

	if (gccPath.empty()) {
		res.addEntry("EUKN", "Failed to find gcc in path", {{}});
		return res;
	}

	// run it!
	Subprocess sub{gccPath};
	sub.setArguments({"-xc", "-E", "-v", "-"});

	// the spicy output is in stderr
	std::string stdErrData;
	sub.attachStringToStdErr(stdErrData);

	res += sub.start();
	if (!res) { return res; }

	res += sub.closeStdIn();
	if (!res) { return res; }

	int err = sub.exitCode();
	if (err != 0) {
		res.addEntry("EUKN", "Failed to run GCC", {{"Error code", err}, {"Stderr", stdErrData}});
		return res;
	}

	// parse the stderr
	// it looks something like this:
	//
	// <random stuff>
	// #include <...> search starts here:
	//  <include path>
	//  ...
	// End of search list.

	// split it into lines
	std::vector<std::string> stdErrLines;
	boost::algorithm::split(stdErrLines, stdErrData, boost::algorithm::is_any_of("\n"));

	// find #include <...> search starts here:
	auto iter =
	    std::find(stdErrLines.begin(), stdErrLines.end(), "#include <...> search starts here:");
	if (iter == stdErrLines.end()) {
		res.addEntry("EUKN", "Failed to find \"#include <...> search starts here:\" in gcc stderr",
		             {{"Stderr", stdErrData}});
		return res;
	}

	// start appending
	++iter;

	for (; iter != stdErrLines.end() && *iter != "End of search list."; ++iter) {
		assert((*iter)[0] == ' ');
		std::string incPath = iter->substr(1);  // remove the first char, it's gonna be a space

		// append it to `toFill`
		if (fs::is_directory(incPath)) { toFill.push_back(incPath); }
	}

	return res;
}
}  // chi

#elif defined WIN32

// it actually works fine without anyting! yay!

#include "chi/Support/Result.hpp"

namespace chi {

Result stdCIncludePaths(std::vector<boost::filesystem::path>& toFill) { return {}; }

}  // namespace chi

#else

#endif
