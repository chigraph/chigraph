/// \file BitcodeParser.cpp

#include "BitcodeParser.hpp"
#include "LLVMVersion.hpp"

#include "chi/Support/Result.hpp"

#if LLVM_VERSION_LESS_EQUAL(3, 9)
#include <llvm/Bitcode/ReaderWriter.h>
#else
#include <llvm/Bitcode/BitcodeReader.h>
#endif

#include <llvm/IR/Module.h>
#include <llvm/Support/MemoryBuffer.h>

namespace chi {

namespace {

template <typename MemBuffType>
Result parseBitcodeMemBuff(MemBuffType buff, llvm::LLVMContext& ctx,
                           std::unique_ptr<llvm::Module>* toFill) {
	assert(toFill != nullptr && "Cannot pass a null toFill pointer to parseBitcodeMemBuff");

	Result res;

	auto errorOrMod = llvm::parseBitcodeFile(buff, ctx);
	if (!errorOrMod) {
		std::vector<std::string> errorMsgs;

#if LLVM_VERSION_AT_LEAST(4, 0)
		auto E = errorOrMod.takeError();

		llvm::handleAllErrors(std::move(E), [&errorMsgs](llvm::ErrorInfoBase& err) {
			errorMsgs.push_back(err.message());
		});
#endif

		res.addEntry("EUKN", "Failed to parse generated bitcode.", {{"Error Messages", errorMsgs}});

		return res;
	}
	*toFill =
#if LLVM_VERSION_LESS_EQUAL(3, 6)
	    std::unique_ptr<llvm::Module>
#else
	    std::move
#endif
	    (errorOrMod.get());

	return res;
}

}  // namespace

Result parseBitcodeFile(const boost::filesystem::path& file, llvm::LLVMContext& ctx,
                        std::unique_ptr<llvm::Module>* toFill) {
	Result res;

	// if all of this is true, then we can read the cache
	auto bcFileBufferOrError = llvm::MemoryBuffer::getFile(file.string());
	if (!bcFileBufferOrError) {
		res.addEntry("EUKN", "Failed to load LLVM module from disk", {{"File", file.string()}});
		return res;
	}

	return parseBitcodeMemBuff(bcFileBufferOrError
	                               .get()
#if LLVM_VERSION_AT_LEAST(3, 6)
	                               ->getMemBufferRef()
#else
	                               .get()
#endif
	                               ,
	                           ctx, toFill);
}
Result parseBitcodeString(const std::string& bitcode, llvm::LLVMContext& ctx,
                          std::unique_ptr<llvm::Module>* toFill) {
	return parseBitcodeMemBuff(
#if LLVM_VERSION_LESS_EQUAL(3, 5)
	    llvm::MemoryBuffer::getMemBufferCopy
#else
	    llvm::MemoryBufferRef
#endif
	    (bitcode, "generated.bc"),
	    ctx, toFill);
}

}  // namespace chi
