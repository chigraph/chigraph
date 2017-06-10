#include "chi/NodeCompiler.hpp"

#include <cassert>

#include "chi/LLVMVersion.hpp"
#include "chi/NodeInstance.hpp"
#include "chi/NodeType.hpp"
#include "chi/FunctionCompiler.hpp"
#include "chi/DataType.hpp"
#include "chi/Context.hpp"
#include "chi/Result.hpp"

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/DIBuilder.h>
#include <llvm/IR/DebugInfo.h>

namespace chi {

NodeCompiler::NodeCompiler(FunctionCompiler& functionCompiler, NodeInstance& inst)
	: mCompiler{&functionCompiler}, mNode{&inst} {
	
	// alloca the outputs
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
		
		mReturnValues.push_back(alloca);
	}
	
	auto size = inputExecs();
	
	// resize the inputexec specific variables
	mPureBlocks.resize(size);
	mCodeBlocks.resize(size, nullptr);
	
	mCompiledInputs.resize(size, false);
}

bool NodeCompiler::pure() const { return node().type().pure(); }

void NodeCompiler::compile_stage1(size_t inputExecID) {
	
	
	// create the code block
	auto& codeBlock = mCodeBlocks[inputExecID];
	
	// if we've already done stage 1 before, then just exit
	if (codeBlock != nullptr) { return; }
	
	codeBlock = llvm::BasicBlock::Create(context().llvmContext(), "node_" + node().stringId() + "__" + std::to_string(inputExecID), &funcCompiler().llFunction());
	
	// only do this for non-pure nodes because pure nodes don't call their dependencies, they are called by the non-pure
	if (!pure()) {
	
		// generate code for all the dependent pures
		auto depPures = dependentPuresRecursive(node());
		
		// set our vector to be the same length
		auto& pureBlocks = mPureBlocks[inputExecID];
		pureBlocks.resize(depPures.size());
		
		// create the first pure block--the loop only creates the next one
		if (!depPures.empty()) {
			pureBlocks[0] = llvm::BasicBlock::Create(context().llvmContext(), "node_" + node().stringId() + "__" + std::to_string(inputExecID) + "__" + depPures[0]->stringId(), &funcCompiler().llFunction());
		}
		
		for (int id = 0; id < depPures.size(); ++id) {
			
			// create a BasicBlock for the next one to br to--if we're on the last one, use the code block
			llvm::BasicBlock* nextBlock = [&]{
				if (id == depPures.size() - 1) {
					return codeBlock;
				}
				pureBlocks[id + 1] = llvm::BasicBlock::Create(context().llvmContext(), "node_" + node().stringId() + "__" + std::to_string(inputExecID) + "__" + depPures[id + 1]->stringId(), &funcCompiler().llFunction());
				return pureBlocks[id + 1];
			}();
			
			// add nextBlock to the list of possible locations for the indirectbr
			funcCompiler().nodeCompiler(*depPures[id])->jumpBackInst()->addDestination(nextBlock);
			
			// set post-pure break to go to the next one
			llvm::IRBuilder<> irBuilder{pureBlocks[id]};
			irBuilder.CreateStore(llvm::BlockAddress::get(nextBlock), &funcCompiler().postPureBreak());
			
			// br to the pure, terminating that BasicBlock
			irBuilder.CreateBr(&funcCompiler().nodeCompiler(*depPures[id])->firstBlock(0));
		}
		
	}
	
}

Result NodeCompiler::compile_stage2(std::vector<llvm::BasicBlock*> trailingBlocks, size_t inputExecID) {
	
	assert(pure() || trailingBlocks.size() == node().outputExecConnections.size() && "Trailing blocks is the wrong size");
	assert(inputExecID < inputExecs());
	
	auto& codeBlock = mCodeBlocks[inputExecID];
	
	// skip if we've already compiled
	if (compiled(inputExecID)) {return {};}
	
	// if we haven't done stage 1, then do it
	if (codeBlock == nullptr) {
		compile_stage1(inputExecID);
	}
	llvm::IRBuilder<> codeBuilder{codeBlock};
	
	// inputs and outputs (inputs followed by outputs)
	std::vector<llvm::Value*> io;
	
	// add inputs
	for (auto idx = 0ull; idx < node().inputDataConnections.size(); ++idx) {
		auto& connection = node().inputDataConnections[idx];
		auto& remoteNode = *connection.first;
		auto remoteID = connection.second;
		
		assert(remoteID < funcCompiler().nodeCompiler(remoteNode)->returnValues().size() && "Internal error: connection to a value doesn't exist");
		
		auto loaded = codeBuilder.CreateLoad(funcCompiler().nodeCompiler(remoteNode)->returnValues()[remoteID]);
		io.push_back(loaded);
		
		assert(io[io.size() - 1]->getType() == node().type().dataInputs()[idx].type.llvmType() && "Internal error: types do not match");
		
	}
	
	// add outputs
	std::copy(mReturnValues.begin(), mReturnValues.end(), std::back_inserter(io));

	// on pure, jump to the set loc
	if (pure()) {
		auto brBlock = llvm::BasicBlock::Create(context().llvmContext(), "node_" + node().stringId() + "_jumpback", &funcCompiler().llFunction());
		llvm::IRBuilder<> builder(brBlock);
		
		mJumpBackInst = builder.CreateIndirectBr(builder.CreateLoad(&funcCompiler().postPureBreak()));
		
		trailingBlocks.resize(1);
		trailingBlocks[0] = brBlock;
	}
	
	// codegen
	Result res =
	    node().type().codegen(*this, *codeBlock, inputExecID, llvm::DebugLoc::get(funcCompiler().nodeLineNumber(node()), 1,
#if LLVM_VERSION_LESS_EQUAL(3, 6)
	                                                          *
#endif
	                                                          &funcCompiler().diFunction()),
	                         io, trailingBlocks);

	mCompiledInputs[inputExecID] = true;
		
	return res;

}

llvm::BasicBlock& NodeCompiler::firstBlock(size_t inputExecID) const {
	
	assert(inputExecID < inputExecs());
	
	if (mPureBlocks[inputExecID].empty()) {
		return codeBlock(inputExecID);
	}
	return *mPureBlocks[inputExecID][0];
}



llvm::BasicBlock& NodeCompiler::codeBlock(size_t inputExecID) const {
	assert(inputExecID < inputExecs());
    return *mCodeBlocks[inputExecID];

}

llvm::Module& NodeCompiler::llvmModule() const {
	return funcCompiler().llvmModule();
}

bool NodeCompiler::compiled(size_t inputExecID) const {
	assert(inputExecID < inputExecs());
    return mCompiledInputs[inputExecID];
}


Context& NodeCompiler::context() const {
	return node().context();
}


size_t NodeCompiler::inputExecs() const {
	if (pure()) {
		return 1;
	} else if (node().type().qualifiedName() == "lang:entry") {
		return 1;
	}
	return node().inputExecConnections.size();
}


std::vector<NodeInstance*> dependentPuresRecursive(const NodeInstance& inst) {
	std::vector<NodeInstance*> ret;
	
	// TODO: remove duplicates in some intellegent way
	for (const auto& conn : inst.inputDataConnections) {
		
		// if it isn't connected (this really shouldn't happen because that would fail validation), then skip.
		if (conn.first == nullptr) {
			continue;
		}
		
		if (conn.first->type().pure()) {
			
			auto deps = dependentPuresRecursive(*conn.first);
			std::copy(deps.begin(), deps.end(), std::back_inserter(ret));
			
			ret.push_back(conn.first);
		}
	}
	
	return ret;
}

} // namespace chi
