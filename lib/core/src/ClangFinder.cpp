#include "chi/ClangFinder.hpp"

#include <boost/preprocessor/stringize.hpp>

#include "chi/Support/ExecutablePath.hpp"
#include "chi/Support/FindProgram.hpp"

// only non llvm-c include, for LLVM_VERSION*
#include <llvm/Config/llvm-config.h>

#include <vector>

namespace fs = std::filesystem;

namespace chi {
#ifdef WIN32
#define SUFFIX ".exe"
#else
#define SUFFIX ""
#endif

fs::path findClang() {
	std::vector<fs::path> fileNames = {
		"clang" SUFFIX,
		std::string("clang-") + BOOST_PP_STRINGIZE(LLVM_VERSION_MAJOR)
#if LLVM_VERSION_MAJOR < 7
		    + "." + BOOST_PP_STRINGIZE(LLVM_VERSION_MINOR)
#endif
		    + SUFFIX
	};

	// 1st location--CHI_CLANG_EXECUTABLE environment variable
	{
		const char* envVar = std::getenv("CHI_CLANG_EXECUTABLE");
		if (envVar != nullptr) { return envVar; }
	}

	// 2st location--current executable path
	{
		auto exeLoc = executablePath().parent_path();

		for (const auto& fn : fileNames) {
			if (fs::is_regular_file(exeLoc / fn)) { return exeLoc / fn; }
		}
	}
	// 3rd location--path
	{
		for (const auto& fn : fileNames) {
			auto possibleLoc = findProgram(fn.string().c_str());
			if (!possibleLoc.empty()) { return possibleLoc; }
		}
	}

	return {};
}
}  // namespace chi
