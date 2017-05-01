#include <stdio.h>

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <clang/Basic/DiagnosticOptions.h>
#include <clang/Basic/TargetInfo.h>
#include <clang/CodeGen/CodeGenAction.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/TextDiagnosticPrinter.h>
#include <clang/Lex/PreprocessorOptions.h>

#if LLVM_VERSION_MAJOR <= 3
#include <llvm/Bitcode/ReaderWriter.h>
#else
#include <llvm/Bitcode/BitcodeWriter.h>
#endif

#include <llvm/IR/Module.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/raw_os_ostream.h>


#if LLVM_VERSION_MAJOR <= 3 && LLVM_VERSION_MINOR <= 7
#include "clang/Frontend/Utils.h"
#include "clang/Basic/DiagnosticOptions.h"
#include "clang/Driver/Compilation.h"
#include "clang/Driver/Driver.h"
#include "clang/Driver/Action.h"
#include "clang/Driver/Options.h"
#include "clang/Driver/Tool.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendDiagnostic.h"
#include "llvm/Option/ArgList.h"
#include "llvm/Support/Host.h"
#endif

#include <boost/program_options.hpp>

using namespace llvm;
using namespace clang;
using namespace std;

// clang 3.7- don't do this right at all. We need it to pass our arg0, and not just "clang" like 3.7 does. 
// This is copy-pasted from LLVM 4.0 with some minor modifications.
#if LLVM_VERSION_MAJOR <= 3 && LLVM_VERSION_MINOR <= 7
using namespace llvm::opt;

std::unique_ptr<CompilerInvocation> createInvocationFromCommandLineBackport (
    ArrayRef<const char *> ArgList,
    IntrusiveRefCntPtr<DiagnosticsEngine> Diags) {
  if (!Diags.get()) {
    // No diagnostics engine was provided, so create our own diagnostics object
    // with the default options.
    Diags = CompilerInstance::createDiagnostics(new DiagnosticOptions);
  }

  SmallVector<const char *, 16> Args(ArgList.begin(), ArgList.end());

  // FIXME: Find a cleaner way to force the driver into restricted modes.
  Args.push_back("-fsyntax-only");

  // FIXME: We shouldn't have to pass in the path info.
  driver::Driver TheDriver(Args[0], llvm::sys::getDefaultTargetTriple(),
                           *Diags);

  // Don't check that inputs exist, they may have been remapped.
  TheDriver.setCheckInputsExist(false);

  std::unique_ptr<driver::Compilation> C(TheDriver.BuildCompilation(Args));

  // Just print the cc1 options if -### was present.
  if (C->getArgs().hasArg(driver::options::OPT__HASH_HASH_HASH)) {
    C->getJobs().Print(llvm::errs(), "\n", true);
    return nullptr;
  }

  // We expect to get back exactly one command job, if we didn't something
  // failed.
  const driver::JobList &Jobs = C->getJobs();
  if (Jobs.size() > 1) {
    for (auto &A : C->getActions()){
      // On MacOSX real actions may end up being wrapped in BindArchAction
      if (isa<driver::BindArchAction>(A))
        A = *A->begin();
    }
  }
  if (Jobs.size() == 0 || !isa<driver::Command>(*Jobs.begin()) ||
      (Jobs.size() > 1)) {
    SmallString<256> Msg;
    llvm::raw_svector_ostream OS(Msg);
    Jobs.Print(OS, "; ", true);
    Diags->Report(diag::err_fe_expected_compiler_job) << OS.str();
    return nullptr;
  }

  const driver::Command &Cmd = 
#if LLVM_VERSION_MAJOR <= 3 && LLVM_VERSION_MINOR <= 5
	*
#endif
  cast<driver::Command>(*Jobs.begin());
  if (StringRef(Cmd.getCreator().getName()) != "clang") {
    Diags->Report(diag::err_fe_expected_clang_command);
    return nullptr;
  }

  const ArgStringList &CCArgs = Cmd.getArguments();
  auto CI = std::make_unique<CompilerInvocation>();
  if (!CompilerInvocation::CreateFromArgs(*CI,
                                     const_cast<const char **>(CCArgs.data()),
                                     const_cast<const char **>(CCArgs.data()) +
                                     CCArgs.size(),
                                     *Diags))
    return nullptr;
  return CI;
}
#endif

std::unique_ptr<llvm::Module> cToLLVM(LLVMContext& ctx, const char* execPath, const char* code,
                                      const char* fileName, std::vector<const char*> compileArgs) {
	// Prepare compilation arguments
	compileArgs.insert(compileArgs.begin(), execPath);
	
	if (strcmp(code, "") != 0) {
		compileArgs.push_back(fileName);
	}

	// Prepare DiagnosticEngine
	auto* DiagOpts = new DiagnosticOptions;

	auto*              textDiagPrinter = new clang::TextDiagnosticPrinter(llvm::errs(), DiagOpts);
	IntrusiveRefCntPtr<DiagnosticIDs> DiagID(new DiagnosticIDs());

	IntrusiveRefCntPtr<DiagnosticsEngine> diagnosticsEngine(
	    new DiagnosticsEngine(DiagID, DiagOpts, textDiagPrinter));

	std::unique_ptr<CompilerInstance> Clang(new CompilerInstance());

	// Initialize CompilerInvocation
	auto invoc =
#if LLVM_VERSION_MAJOR >= 4
	    std::shared_ptr<clang::CompilerInvocation>
#endif
	    (
#if LLVM_VERSION_MAJOR <= 3 && LLVM_VERSION_MINOR <= 7
			createInvocationFromCommandLineBackport
#else
			clang::createInvocationFromCommandLine
#endif
			(compileArgs, diagnosticsEngine)
		);

	Clang->setInvocation(invoc
#if LLVM_VERSION_MAJOR <= 3 && LLVM_VERSION_MINOR <= 7
		.get()
#endif 
	);

	// Map code filename to a memoryBuffer
	unique_ptr<MemoryBuffer> buffer;
	if (strcmp(code, "") != 0) {
		StringRef                testCodeData(code);
		buffer = 
#if LLVM_VERSION_MAJOR <= 3 && LLVM_VERSION_MINOR <= 5
			std::unique_ptr<MemoryBuffer>
#endif
			(MemoryBuffer::getMemBufferCopy(testCodeData));
		Clang->getInvocation().getPreprocessorOpts().addRemappedFile(fileName, buffer.get());
	}
	
	// Create and initialize CompilerInstance
	Clang->createDiagnostics();

	// Create and execute action
	auto compilerAction = std::make_unique<EmitLLVMOnlyAction>(&ctx);
	Clang->ExecuteAction(*compilerAction);
	
	buffer.release();

	return 
#if LLVM_VERSION_MAJOR <= 3 && LLVM_VERSION_MINOR <= 5
		std::unique_ptr<llvm::Module>
#endif
		(compilerAction->takeModule());
}

namespace po = boost::program_options;

int main(int argc, char** argv) {
	
	po::options_description general(
	    "chi-ctollvm: Compile C/C++ code from stdin and spit out LLVM bitcode", 50);

	general.add_options()("help,h", "Produce Help Message")
		("clangargs,c", po::value<std::vector<std::string>>()->default_value({}, ""), "Extra arguments to pass to clang")
		("fakename,f", po::value<std::string>()->default_value("internal.c"));
	
	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).options(general).run(), vm);
	po::notify(vm);
		
	// read code from stdin
	std::string code;
	{
		std::string line;
		while(std::getline(std::cin, line)) {
			code += line;
			code += '\n';
		}
	}
	
	std::vector<const char*> strArgs;
	for (const auto& str : vm["clangargs"].as<std::vector<std::string>>()) {
		strArgs.push_back(str.c_str());
	}
	
	llvm::LLVMContext ctx;
	auto mod = cToLLVM(ctx, argv[0], code.c_str(), vm["fakename"].as<std::string>().c_str(), strArgs);
	
	llvm::WriteBitcodeToFile(mod.get(), llvm::outs());
}
