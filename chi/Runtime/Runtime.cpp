#include "Runtime.hpp"

#include "chi/Support/Result.hpp"

#include "chi/CodeSupport/BitcodeParser.hpp"

#include <fstream>

std::string embedded_chigraph_runtime_bitcode();

namespace chi {
Result chigraphRuntime(llvm::LLVMContext& ctx, std::unique_ptr<llvm::Module>* toFill) {
    Result res;
    res.addContext({
        {"Module", "Runtime"}
    });
    std::ofstream str("/home/russellg/a.bc", std::ios::binary);

    str << embedded_chigraph_runtime_bitcode();

    res += parseBitcodeString(embedded_chigraph_runtime_bitcode(), ctx, toFill);

    return res;
}

} // namespace chi
