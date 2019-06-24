#include "chi/ClangFinder.hpp"
#include "chi/Support/ExecutablePath.hpp"
#include "chi/Support/FindProgram.hpp"

#include <boost/preprocessor/stringize.hpp>

// only non llvm-c include, for LLVM_VERSION*
#include <llvm/Config/llvm-config.h>

namespace fs = std::filesystem;

namespace chi {

fs::path findClang() {
	fs::path fileName = std::string("clang-") + BOOST_PP_STRINGIZE(LLVM_VERSION_MAJOR)
#if LLVM_VERSION_MAJOR < 7
	                    + "." + BOOST_PP_STRINGIZE(LLVM_VERSION_MINOR)
#endif
#ifdef WIN32
	                    + ".exe"
#endif
	    ;

	// 1st location--CHI_CLANG_EXECUTABLE environment variable
	{
		const char* envVar = std::getenv("CHI_CLANG_EXECUTABLE");
		if (envVar != nullptr) { return envVar; }
	}

	// 2st location--current executable path
	{
		auto exeLoc = executablePath().parent_path();

		if (fs::is_regular_file(exeLoc / fileName)) { return exeLoc / fileName; }
	}
	// 3rd location--path
	{
		auto possibleLoc = findProgram(fileName.c_str());
		if (!possibleLoc.empty()) { return possibleLoc; }
	}
	// 4th location--path without version
	{
		auto possibleLoc = findProgram("clang");
		if (!possibleLoc.empty()) { return possibleLoc; }
	}

	return {};
}
}  // namespace chi
