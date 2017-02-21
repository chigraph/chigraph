#include <llvm/IR/Module.h>
#include <llvm/IR/LLVMContext.h>

#include <vector>
#include <memory>
#include <string>

std::unique_ptr<llvm::Module> cToLLVM(llvm::LLVMContext& ctx, const char* code, const char* fileName, std::vector<const char*> compileArgs, std::string& err);
