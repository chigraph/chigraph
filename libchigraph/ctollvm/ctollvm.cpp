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

#include <llvm/IR/Module.h>
#include <llvm/Support/raw_os_ostream.h>

using namespace llvm;
using namespace clang;
using namespace std;

std::unique_ptr<llvm::Module> cToLLVM(LLVMContext& ctx, const char* execPath, const char* code,
                                      const char* fileName, std::vector<const char*> compileArgs,
                                      std::string& err) {
	// Prepare compilation arguments
	compileArgs.insert(compileArgs.begin(), execPath);
	
	if (strcmp(code, "") != 0) {
		compileArgs.push_back(fileName);
	}

	// Prepare DiagnosticEngine
	auto* DiagOpts = new DiagnosticOptions;

	raw_string_ostream errStream{err};
	auto*              textDiagPrinter = new clang::TextDiagnosticPrinter(errStream, DiagOpts);
	IntrusiveRefCntPtr<DiagnosticIDs> DiagID(new DiagnosticIDs());

	IntrusiveRefCntPtr<DiagnosticsEngine> diagnosticsEngine(
	    new DiagnosticsEngine(DiagID, DiagOpts, textDiagPrinter));

	std::unique_ptr<CompilerInstance> Clang(new CompilerInstance());

	// Initialize CompilerInvocation
	auto invoc =
#if LLVM_VERSION_MAJOR >= 4
	    std::shared_ptr<clang::CompilerInvocation>
#endif
	    (clang::createInvocationFromCommandLine(compileArgs, diagnosticsEngine));

	Clang->setInvocation(invoc);

	// Map code filename to a memoryBuffer
	unique_ptr<MemoryBuffer> buffer;
	if (strcmp(code, "") != 0) {
		StringRef                testCodeData(code);
		buffer = MemoryBuffer::getMemBufferCopy(testCodeData);
		Clang->getInvocation().getPreprocessorOpts().addRemappedFile(fileName, buffer.get());
	}
	
	// Create and initialize CompilerInstance
	Clang->createDiagnostics();

	// Create and execute action
	auto compilerAction = std::make_unique<EmitLLVMOnlyAction>(&ctx);
	Clang->ExecuteAction(*compilerAction);
	
	buffer.release();

	return compilerAction->takeModule();
}
