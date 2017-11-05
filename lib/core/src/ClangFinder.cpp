#include "chi/ClangFinder.hpp"

#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Program.h>

#include <boost/filesystem/operations.hpp>
#include <boost/preprocessor/stringize.hpp>

namespace fs = boost::filesystem;

namespace chi {

fs::path findClang() {
	fs::path fileName = std::string("clang-") + BOOST_PP_STRINGIZE(LLVM_VERSION_MAJOR) + "." +
	                    BOOST_PP_STRINGIZE(LLVM_VERSION_MINOR)
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
		auto exeLoc = fs::path(llvm::sys::fs::getMainExecutable(nullptr, nullptr)).parent_path();

		if (fs::is_regular_file(exeLoc / fileName)) { return exeLoc / fileName; }
	}
	// 3nd location--path
	{
		auto possibleLoc = llvm::sys::findProgramByName(fileName.string());
		if (possibleLoc) { return *possibleLoc; }
	}
	// 4nd location--path without version
	{
		auto possibleLoc = llvm::sys::findProgramByName("clang");
		if (possibleLoc) { return *possibleLoc; }
	}

	return {};
}
}  // namespace chi
