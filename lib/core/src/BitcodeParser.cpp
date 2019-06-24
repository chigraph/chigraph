/// \file BitcodeParser.cpp

#include "chi/BitcodeParser.hpp"
#include "chi/Support/Result.hpp"

#include <llvm-c/BitReader.h>
#include <llvm-c/Core.h>
#include <llvm-c/Error.h>

namespace chi {

namespace {

Result parseBitcodeMemBuff(LLVMMemoryBufferRef buff, LLVMContextRef ctx, OwnedLLVMModule* toFill) {
	assert(toFill != nullptr && "Cannot pass a null toFill pointer to parseBitcodeMemBuff");

	Result res;

	LLVMModuleRef module;
	OwnedMessage  message;
	if (LLVMParseBitcodeInContext(ctx, buff, &module, &*message)) {
		res.addEntry("EUKN", "Failed to parse generated bitcode.", {{"Error Message", *message}});

		return res;
	}
	*toFill = OwnedLLVMModule(module);

	return res;
}  // namespace

}  // namespace

Result parseBitcodeFile(const std::filesystem::path& file, LLVMContextRef ctx,
                        OwnedLLVMModule* toFill) {
	Result res;

	// if all of this is true, then we can read the cache
	OwnedMessage          message;
	OwnedLLVMMemoryBuffer buffer;
	if (LLVMCreateMemoryBufferWithContentsOfFile(file.c_str(), &*buffer, &*message)) {
		res.addEntry("EUKN", "Failed to load LLVM module from disk",
		             {{"File", file.string()}, {"Error Message", *message}});

		return res;
	}
	return parseBitcodeMemBuff(*buffer, ctx, toFill);
}  // namespace chi
Result parseBitcodeString(const std::string& bitcode, LLVMContextRef ctx, OwnedLLVMModule* toFill) {
	return parseBitcodeMemBuff(*OwnedLLVMMemoryBuffer(LLVMCreateMemoryBufferWithMemoryRange(
	                               bitcode.data(), bitcode.length(), "generated.bc", false)),
	                           ctx, toFill);
}

}  // namespace chi
