#include "chi/PureCompiler.hpp"

#include "chi/NodeInstance.hpp"
#include "chi/GraphFunction.hpp"

namespace chi {

PureCompiler::PureCompiler(GraphFunction& func, llvm::Module& codegenInto)
	: mLLModule{&codegenInto}, mFunction{&func}, mContext{&func.context()} {}

void PureCompiler::updatePure(NodeInstance& dependentNode, NodeInstance& pureNode, llvm::BasicBlock& codegenInto) {
	// see if it's already cached
	
}

} // namespace chi
