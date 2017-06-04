#include "chi/NodeCompiler.hpp"

#include <cassert>

#include "chi/LLVMVersion.hpp"
#include "chi/NodeInstance.hpp"
#include "chi/NodeType.hpp"
#include "chi/FunctionCompiler.hpp"
#include "chi/DataType.hpp"
#include "chi/Context.hpp"

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/DIBuilder.h>
#include <llvm/IR/DebugInfo.h>

namespace chi {

NodeCompiler::NodeCompiler(FunctionCompiler& funcCompiler, NodeInstance& inst) 
	: mCompiler{&funcCompiler}, mNode{&inst} {
}

bool NodeCompiler::pure() const { return node().type().pure(); }

void NodeCompiler::compile(const std::vector<llvm::BasicBlock>& trailingBlocks) {
	
	assert(pure() || trailingBlocks.size() == node().outputExecConnections.size() && "Trailing blocks is the wrong size");
	
	// create the code block
	mCodeBlock = llvm::BasicBlock::Create(context().llvmContext(), node().stringId(), &funcCompiler().llFunction());
	
	// only do this for non-pure nodes because pure nodes don't call their dependencies, they are called by the non-pure
	if (!pure()) {
	
		// generate code for all the dependent pures
		auto depPures = dependentPuresRecursive(node());
		
		// set our vector to be the same length
		mPureBlocks.resize(depPures.size());
		
		// create the first pure block--the loop only creates the next one
		if (!depPures.empty()) {
			mPureBlocks[0] = llvm::BasicBlock::Create(context().llvmContext(), node().stringId() + "__" + depPures[0]->stringId(), &funcCompiler().llFunction());
		}
		
		for (int id = 0; id < depPures.size(); ++id) {
			
			// create a BasicBlock for the next one to br to--if we're on the last one, use the code block
			llvm::BasicBlock* nextBlock = [&]{
				if (id == depPures.size() - 1) {
					return &codeBlock();
				}
				mPureBlocks[id + 1] = mPureBlocks[0] = llvm::BasicBlock::Create(context().llvmContext(), node().stringId() + "__" + depPures[id + 1]->stringId(), &funcCompiler().llFunction());
				return mPureBlocks[id + 1];
			}();
			
			// set post-pure break to go to the next one
			llvm::IRBuilder<> irBuilder{mPureBlocks[id]};
			irBuilder.CreateStore(llvm::BlockAddress::get(nextBlock), &funcCompiler().postPureBreak());
			
			// br to the pure, terminating that BasicBlock
			irBuilder.CreateBr(&funcCompiler().getOrCompileNode(*depPures[id]).firstBlock());
		}
		
	}
	
	// inputs and outputs (inputs followed by outputs)
	std::vector<llvm::Value*> io;
	
	// add inputs
	for (auto idx = 0ull; idx < node().inputDataConnections.size(); ++idx) {
		auto& connection = node().inputDataConnections[idx];
		auto& remoteNode = *connection.first;
		auto remoteID = connection.second;
		
		assert(remoteID < funcCompiler().getOrCompileNode(remoteNode).returnValues().size() && "Internal error: connection to a value doesn't exist");
		
		io.push_back(funcCompiler().getOrCompileNode(remoteNode).returnValues()[remoteID]);
		
		assert(io[io.size() - 1]->getType() == node().type().dataInputs()[idx].type.llvmType() && "Internal error: types do not match");
		
	}
	
	// add outputs
	llvm::IRBuilder<> allocBuilder(&funcCompiler().allocBlock());
	for (auto idx = 0ull; idx < node().type().dataOutputs().size(); ++idx) {
		const auto& namedType = node().type().dataOutputs()[idx];
		
		// alloca the outputs
		auto alloca = allocBuilder.CreateAlloca(namedType.type.llvmType(), nullptr, node().stringId() + "__" + std::to_string(idx));
		
		// create debug info for the alloca
		{
			// get type
			llvm::DIType* dType = namedType.type.debugType();

			// TODO(#63): better names
			auto debugVar =
				funcCompiler().diBuilder().
#if LLVM_VERSION_LESS_EQUAL(3, 7)
				createLocalVariable(
					llvm::dwarf::DW_TAG_auto_variable,
#else
				createAutoVariable(
#endif
#if LLVM_VERSION_LESS_EQUAL(3, 6)
					*
#endif
					&funcCompiler().diFunction(), node().stringId() + "__" + std::to_string(idx),
#if LLVM_VERSION_LESS_EQUAL(3, 6)
					data.dbuilder->createFile(
						fs::path(data.diFunc->getFilename()).filename().string(),
						fs::path(data.diFunc->getFilename()).parent_path().string()),
#else
					funcCompiler().diFunction().getFile(),
#endif
					1,
#if LLVM_VERSION_LESS_EQUAL(3, 6)
					*
#endif
					dType);

			funcCompiler().diBuilder().insertDeclare(alloca, debugVar,
#if LLVM_VERSION_AT_LEAST(3, 6)
								funcCompiler().diBuilder().createExpression(),
#if LLVM_VERSION_AT_LEAST(3, 7)
								llvm::DebugLoc::get(1, 1, &funcCompiler().diFunction()),
#endif
#endif
								&funcCompiler().allocBlock())
#if LLVM_VERSION_LESS_EQUAL(3, 6)
				->setDebugLoc(llvm::DebugLoc::get(1, 1, *data.diFunc))
#endif
				;
		}
		
		// add it to the io vector and return list
		io.push_back(alloca);
		mReturnValues.push_back(alloca);
	}

	// codegen
	res +=
	    node().type().codegen(execInputID, llvm::DebugLoc::get(data.nodeLocations.right.at(node), 1,
#if LLVM_VERSION_LESS_EQUAL(3, 6)
	                                                          *
#endif
	                                                          data.diFunc),
	                         io, codeBlock, outputBlocks, data.compileCache);
	if (!res) { return {boost::dynamic_bitset<>{}, std::vector<llvm::BasicBlock*>{}}; }

	// TODO(#64): raise an error here instead
	for (auto& bb : unusedBlocks) {
		llvm::IRBuilder<> builder(bb);

		builder.CreateRet(
		    llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(mod->getContext()), 0, true));
	}

	data.nodeCache[node].lastNodeCodegenned = lastImpure;

	return {needsCodegen, outputBlocks};
}

llvm::BasicBlock& NodeCompiler::firstBlock() const {
	assert(compiled());
	
	if (mPureBlocks.empty()) {
		return codeBlock();
	}
	return *mPureBlocks[0];
}


std::vector<NodeInstance*> dependentPuresRecursive(const NodeInstance& inst) {
	std::vector<NodeInstance*> ret;
	
	for (const auto& conn : inst.inputDataConnections()) {
		
		// if it isn't connected (this really shouldn't happen because that would fail validation), then skip.
		if (conn.first == nullptr) {
			continue;
		}
		
		if (conn.first->type().pure()) {
			
			auto deps = dependentPuresRecursive(conn.first);
			std::copy(deps.begin(), deps.end(), std::back_inserter(ret));
			
			ret.push_back(conn.first);
		}
	}
	
	return ret;
}

Context& NodeCompiler::context() const {
	return node().context();
}

} // namespace chi
