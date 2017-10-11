/// \file CCompiler.cpp

#include "chi/CCompiler.hpp"
#include "chi/LLVMVersion.hpp"
#include "chi/BitcodeParser.hpp"
#include "chi/Support/LibCLocator.hpp"
#include "chi/Support/Result.hpp"
#include "chi/Support/Subprocess.hpp"

#include <llvm/IR/Module.h>
#include <llvm/Support/MemoryBuffer.h>

namespace fs = boost::filesystem;

namespace chi {

Result compileCToLLVM(const boost::filesystem::path& ctollvmPath, llvm::LLVMContext& llvmContext,
                      std::vector<std::string> arguments, boost::string_view inputCCode,
                      std::unique_ptr<llvm::Module>* toFill) {
	assert(toFill != nullptr && "null toFill passed to compileCToLLVM");
	assert(fs::is_regular_file(ctollvmPath) &&
	       "invalid path passed to compileCToLLVM for ctollvmPath");

	Result res;

	arguments.push_back("-nostdlib");

	// gather std include paths
	std::vector<fs::path> stdIncludePaths;
	res += stdCIncludePaths(stdIncludePaths);
	if (!res) { return res; }

	for (const auto& p : stdIncludePaths) {
		arguments.push_back("-I");
		arguments.push_back(p.string());
	}

	std::string errors;

	// call chi-ctollvm
	std::unique_ptr<llvm::Module> mod;
	{
		// chi-ctollvm accepts args in the style of -c <arg1> -c <arg2> ...
		std::vector<std::string> argsToChiCtoLLVM;
		for (const auto& arg : arguments) {
			argsToChiCtoLLVM.push_back("-c");
			argsToChiCtoLLVM.push_back(arg);
		}

		std::string generatedBitcode;
		Subprocess  ctollvmExe(ctollvmPath);
		ctollvmExe.setArguments(argsToChiCtoLLVM);

		ctollvmExe.attachStringToStdOut(generatedBitcode);
		ctollvmExe.attachStringToStdErr(errors);

		res += ctollvmExe.start();

		// push it the code and close the stream
		res += ctollvmExe.pushToStdIn(inputCCode.data(), inputCCode.size());
		res += ctollvmExe.closeStdIn();

		if (!res) { return res; }

		// wait for the exit
		auto errCode = ctollvmExe.exitCode();

		if (errCode != 0) {
			res.addEntry("EUKN", "Failed to Generate IR with clang", {{"Error", errors}});
			return res;
		}
		if (!errors.empty()) {
			res.addEntry("WUKN", "Warnings emitted while generating IR with clang",
			             {{"Warning", errors}});
		}

		auto readCtx = res.addScopedContext({{"Error parsing bitcode file generated from clang", &inputCCode[0]}});
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

}  // namepsace chi
