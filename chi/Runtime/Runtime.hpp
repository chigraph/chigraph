/// \file Runtime.hpp

#ifndef CHI_RUNTIME_HPP
#define CHI_RUNTIME_HPP

#pragma once

#include "chi/Support/Fwd.hpp"

#include "chi/CodeSupport/Fwd.hpp"

#include <memory>

namespace chi {
    Result chigraphRuntime(llvm::LLVMContext& ctx, std::unique_ptr<llvm::Module>* toFill);
}

#endif // CHI_RUNTIME_HPP
