#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

#include <memory>
#include <string>
#include <vector>

std::unique_ptr<llvm::Module> cToLLVM(llvm::LLVMContext& ctx, const char* execPath, const char* code,
                                      const char* fileName, std::vector<const char*> compileArgs,
                                      std::string& err);
