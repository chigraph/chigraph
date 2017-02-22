#include <iostream>
#include <string>
#include <vector>

#include <clang/Frontend/CompilerInstance.h>
#include <clang/Basic/DiagnosticOptions.h>
#include <clang/Frontend/TextDiagnosticPrinter.h>
#include <clang/CodeGen/CodeGenAction.h>
#include <clang/Basic/TargetInfo.h>
#include <clang/Lex/PreprocessorOptions.h>

#include <llvm/IR/Module.h>
#include <llvm/Support/raw_os_ostream.h>

using namespace llvm;
using namespace clang;
using namespace std;

std::unique_ptr<llvm::Module> cToLLVM(LLVMContext& ctx, const char* code, const char* fileName, std::vector<const char*> compileArgs, std::string& err) {


    // Prepare compilation arguments
    // TODO: fix this please
    compileArgs.insert(compileArgs.begin(), "/home/russellg/projects/llvm-release/bin/clang");
    compileArgs.push_back(fileName);

    // Prepare DiagnosticEngine 
    auto* DiagOpts = new DiagnosticOptions;
    
    raw_string_ostream errStream{err};
    auto* textDiagPrinter =
            new clang::TextDiagnosticPrinter(errStream,
                                         DiagOpts);  
    IntrusiveRefCntPtr<DiagnosticIDs> DiagID(new DiagnosticIDs());

    IntrusiveRefCntPtr<DiagnosticsEngine> diagnosticsEngine(new DiagnosticsEngine(DiagID,
                                         DiagOpts,
                                         textDiagPrinter));

    std::unique_ptr<CompilerInstance> Clang(new CompilerInstance());
  
    // Initialize CompilerInvocation
    auto invoc = std::shared_ptr<clang::CompilerInvocation>(clang::createInvocationFromCommandLine(compileArgs, diagnosticsEngine));
    Clang->setInvocation(invoc);
    
    // Map code filename to a memoryBuffer
    StringRef testCodeData(code);
    unique_ptr<MemoryBuffer> buffer = MemoryBuffer::getMemBufferCopy(testCodeData);
    Clang->getInvocation().getPreprocessorOpts().addRemappedFile(fileName, buffer.get());


    // Create and initialize CompilerInstance
    Clang->createDiagnostics();


    // Create and execute action
    auto compilerAction = std::make_unique<EmitLLVMOnlyAction>(&ctx);
    Clang->ExecuteAction(*compilerAction);
    
    buffer.release();
    

    return compilerAction->takeModule();
    
}
