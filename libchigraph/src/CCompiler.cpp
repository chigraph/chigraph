/// \file CCompiler.cpp

#include "chi/CCompiler.hpp"
#include "chi/Result.hpp"
#include "chi/LibCLocator.hpp"
#include "chi/Subprocess.hpp"
#include "chi/LLVMVersion.hpp"

#if LLVM_VERSION_LESS_EQUAL(3, 9)
#include <llvm/Bitcode/ReaderWriter.h>
#else
#include <llvm/Bitcode/BitcodeReader.h>
#endif

namespace fs = boost::filesystem;

namespace chi {

Result compileCToLLVM(const boost::filesystem::path& ctollvmPath, llvm::LLVMContext& llvmContext, std::vector<std::string> arguments, boost::string_view inputCCode, std::unique_ptr<llvm::Module>* toFill) {

	assert(toFill != nullptr && "null toFill passed to compileCToLLVM");
	assert(fs::is_regular_file(ctollvmPath) &&
	       "invalid path passed to compileCToLLVM for ctollvmPath");

	Result res;

	arguments.push_back("-nostdlib");

	// gather std include paths
	std::vector<fs::path> stdIncludePaths;
	res += stdCIncludePaths(stdIncludePaths);
	if (!res) {
		return res;
	}

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
		Subprocess ctollvmExe(ctollvmPath);
		ctollvmExe.setArguments(argsToChiCtoLLVM);

		ctollvmExe.attachStringToStdOut(generatedBitcode);
		ctollvmExe.attachStringToStdErr(errors);

		res += ctollvmExe.start();

		// push it the code and close the stream
		res += ctollvmExe.pushToStdIn(inputCCode.data(), inputCCode.size());
		res += ctollvmExe.closeStdIn();

		if (!res) {
			return res;
		}

		// wait for the exit
		auto errCode = ctollvmExe.exitCode();

		if (errCode != 0) {
			res.addEntry("EUKN", "Failed to Generate IR with clang", {{"Error", errors}});
			return res;
		}
		if (!errors.empty()) {
			res.addEntry("WUKN", "Failed to generate IR with clang", {{"Warning", errors}});
		}

		auto errorOrMod = llvm::parseBitcodeFile(
#if LLVM_VERSION_LESS_EQUAL(3, 5)
		    llvm::MemoryBuffer::getMemBufferCopy
#else
		    llvm::MemoryBufferRef
#endif
		    (generatedBitcode, "generated.bc"),
		    llvmContext);
		if (!errorOrMod) {
			std::string errorMsg;

#if LLVM_VERSION_AT_LEAST(4, 0)
			auto E = errorOrMod.takeError();

			llvm::handleAllErrors(
			    std::move(E), [&errorMsg](llvm::ErrorInfoBase& err) { errorMsg = err.message(); });
#endif

			res.addEntry("EUKN", "Failed to parse generated bitcode.",
			             {{"Error Message", errorMsg}});

			return res;
		}
		*toFill =
#if LLVM_VERSION_LESS_EQUAL(3, 6)
		    std::unique_ptr<llvm::Module>
#else
		    std::move
#endif
		    (errorOrMod.get());
	}

	if (*toFill == nullptr) {
		res.addEntry("EUKN", "Failed to generate IR with clang", {{"Error", errors}});
	} else if (!errors.empty()) {
		res.addEntry("WUKN", "Warnings encountered while generating IR with clang",
		             {{"Error", errors}});
	}

	return res;

}

} // namepsace chi
