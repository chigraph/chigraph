/// \file CCompiler.cpp

#include "chi/CCompiler.hpp"

#include "chi/BitcodeParser.hpp"
#include "chi/Support/LibCLocator.hpp"
#include "chi/Support/Result.hpp"
#include "chi/Support/Subprocess.hpp"

namespace fs = std::filesystem;

namespace chi {

Result compileCToLLVM(const std::filesystem::path& clangPath, LLVMContextRef llvmContext,
                      std::vector<std::string> arguments, std::string_view inputCCode,
                      OwnedLLVMModule* toFill) {
	assert(toFill != nullptr && "null toFill passed to compileCToLLVM");
	assert(fs::is_regular_file(clangPath) && "invalid path passed to compileCToLLVM for clangPath");

	Result res;

	// gather std include paths
	std::vector<fs::path> stdIncludePaths;
	res += stdCIncludePaths(stdIncludePaths);
	if (!res) { return res; }

	for (const auto& p : stdIncludePaths) {
		arguments.push_back("-I");
		arguments.push_back(p.string());
	}

	arguments.push_back("-nostdlib");

	if (!inputCCode.empty()) {
		arguments.emplace_back("-x");
		arguments.emplace_back("c");
		arguments.emplace_back("-");
	}

	arguments.emplace_back("-c");
	arguments.emplace_back("-emit-llvm");
	arguments.emplace_back("-o");
	arguments.emplace_back("-");

	auto argumentsContext = res.addScopedContext({{"clang arguments", arguments}});

	std::string errors;

	// call clang
	OwnedLLVMModule mod;
	{
		std::string generatedBitcode;
		Subprocess  clangExe(clangPath);
		clangExe.setArguments(arguments);

		clangExe.attachStringToStdOut(generatedBitcode);
		clangExe.attachStringToStdErr(errors);

		res += clangExe.start();

		// push it the code and close the stream
		res += clangExe.pushToStdIn(inputCCode.data(), inputCCode.size());
		res += clangExe.closeStdIn();

		if (!res) { return res; }

		// wait for the exit
		auto errCode = clangExe.exitCode();

		if (errCode != 0) {
			res.addEntry("EUKN", "Failed to Generate IR with clang", {{"Error", errors}});
			return res;
		}
		if (!errors.empty()) {
			res.addEntry("WUKN", "Warnings emitted while generating IR with clang",
			             {{"Warning", errors}});
		}

		auto readCtx = res.addScopedContext(
		    {{"Error parsing bitcode file generated from clang", &inputCCode[0]}});
		res += parseBitcodeString(generatedBitcode, llvmContext, toFill);
	}

	if (*toFill == nullptr) {
		res.addEntry("EUKN", "Failed to generate IR with clang", {{"Error", errors}});
	} else if (!errors.empty()) {
		res.addEntry("WUKN", "Warnings encountered while generating IR with clang",
		             {{"Error", errors}});
	}

	return res;
}

}  // namespace chi
