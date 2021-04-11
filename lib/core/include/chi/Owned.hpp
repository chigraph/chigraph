/// \file Owned.hpp
#pragma once

#include <llvm-c/Core.h>
#include <llvm-c/DebugInfo.h>
#include <llvm-c/ExecutionEngine.h>
#include <llvm-c/Transforms/PassManagerBuilder.h>

#include "chi/Fwd.hpp"

#include <cstddef>
#include <utility>

namespace chi {
namespace detail {
template <typename T>
using Deleter = void (*)(T);
}

/// An owned object. It takes in the type that thie object that it wraps, and the deleter function
/// This is a no-cost abstraction.
template <typename T, detail::Deleter<T> Deleter>
class Owned {
public:
	Owned() : mObject(nullptr) {}
	Owned(std::nullptr_t) : Owned() {}
	explicit Owned(T object) : mObject(object) {}

	// move only
	Owned(Owned&& other) { std::swap(this->mObject, other.mObject); }
	Owned(const Owned&) = delete;

	Owned& operator=(Owned&& other) {
		std::swap(this->mObject, other.mObject);
		return *this;
	}
	Owned& operator=(const Owned&) = delete;

	bool operator==(const Owned& other) { return this->mObject == other.mObject; }

	operator bool() { return mObject != nullptr; }

	~Owned() {
		if (mObject) {
			Deleter(mObject);
			mObject = nullptr;
		}
	}

	const T& operator*() const { return mObject; }
	T&       operator*() { return mObject; }

	T take_ownership() {
		auto ret = mObject;
		mObject  = nullptr;
		return ret;
	}

private:
	T mObject = nullptr;
};

using OwnedLLVMModule             = Owned<LLVMModuleRef, LLVMDisposeModule>;
using OwnedLLVMGenericValue       = Owned<LLVMGenericValueRef, LLVMDisposeGenericValue>;
using OwnedLLVMContext            = Owned<LLVMContextRef, LLVMContextDispose>;
using OwnedMessage                = Owned<char*, LLVMDisposeMessage>;
using OwnedLLVMExecutionEngine    = Owned<LLVMExecutionEngineRef, LLVMDisposeExecutionEngine>;
using OwnedLLVMBuilder            = Owned<LLVMBuilderRef, LLVMDisposeBuilder>;
using OwnedLLVMDIBuilder          = Owned<LLVMDIBuilderRef, LLVMDisposeDIBuilder>;
using OwnedLLVMMemoryBuffer       = Owned<LLVMMemoryBufferRef, LLVMDisposeMemoryBuffer>;
using OwnedLLVMPassManager        = Owned<LLVMPassManagerRef, LLVMDisposePassManager>;
using OwnedLLVMPassManagerBuilder = Owned<LLVMPassManagerBuilderRef, LLVMPassManagerBuilderDispose>;

}  // namespace chi
