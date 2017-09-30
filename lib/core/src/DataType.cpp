/// \file DataType.cpp

#include "chi/ChiModule.hpp"
#include "chi/DataType.hpp"
#include "chi/Context.hpp"

#include <llvm/IR/DerivedTypes.h>

namespace chi {

DataType::DataType(ChiModule* chiMod, std::string typeName, llvm::Type* llvmtype,
                   llvm::DIType* debugTy, bool reference) noexcept
    : mModule(chiMod),
      mName{typeName},
      mLLVMType{llvmtype},
      mDIType{debugTy},
      mReference{reference} {
	assert((!reference || llvmType() == llvm::PointerType::get(
	                                         llvm::Type::getVoidTy(module().context().llvmContext()), 0)) &&
	                           "References must be a void* type");
}

std::string DataType::qualifiedName() const noexcept {
	return module().fullName() + ":" + unqualifiedName();
}

}  // namespace chi
