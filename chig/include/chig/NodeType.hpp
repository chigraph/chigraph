#ifndef CHIG_NODE_TYPE_HPP
#define CHIG_NODE_TYPE_HPP

#pragma once

#include "chig/Context.hpp"
#include "chig/json.hpp"

#include <iterator>
#include <utility>

#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>

namespace chig
{
// generic type
struct NodeType {
	NodeType(Context& con) : context{&con} {}
	virtual ~NodeType() = default;

	std::string name;
	std::string module;
	std::string description;

	Context* context;

	// inputs and outputs
	std::vector<std::pair<llvm::Type*, std::string>> dataInputs;

	std::vector<std::pair<llvm::Type*, std::string>> dataOutputs;

	std::vector<std::string> execInputs;
	std::vector<std::string> execOutputs;

	/// A virtual function that is called when this node needs to be called
	/// \param execInputID The ID of the exec input
	/// \param io This has the values that are the inputs and outputs of the function.
	/// This vector will always have the size of `inputs.size() + outputs.size()` and starts with
	/// the inputs.
	/// The types are gaurenteed to be the same as inputs and outputs
	/// \param codegenInto The IRBuilder object that is used to place calls into
	/// \param outputBlocks The blocks that can be outputted. This will be the same size as
	/// numOutputExecs.
	virtual void codegen(size_t execInputID, llvm::Function* f, const std::vector<llvm::Value*>& io,
		llvm::BasicBlock* codegenInto,
		const std::vector<llvm::BasicBlock*>& outputBlocks) const = 0;
	virtual nlohmann::json toJSON() const { return {}; }
	/// Clones the type
	///
	virtual std::unique_ptr<NodeType> clone() const = 0;
};

struct FunctionCallNodeType : NodeType {
	FunctionCallNodeType(Context& context, llvm::Module* argModule, llvm::Function* func,
		int num_inputs, int numExecOutputs, std::string argDescription,
		const std::vector<std::string>& iodescs)
		: NodeType(context), function{func}
	{
		module = argModule->getName();

		// TODO: gather metadata for exec descs
		execOutputs.resize(numExecOutputs);

		name = func->getName();
		description = std::move(argDescription);

		// populate inputs and outputs
		dataInputs.resize(num_inputs);
		auto beginningOfOutptus = func->getArgumentList().begin();
		std::advance(beginningOfOutptus, num_inputs);

		std::transform(func->getArgumentList().begin(), beginningOfOutptus, iodescs.begin(),
			dataInputs.begin(),
			[](auto& arg, auto& desc) { return std::make_pair(arg.getType(), desc); });

		int num_outputs =
			std::distance(func->getArgumentList().begin(), func->getArgumentList().end()) -
			num_inputs;

		dataOutputs.resize(num_outputs);
		std::transform(beginningOfOutptus, func->getArgumentList().end(),
			iodescs.begin() + num_inputs, dataOutputs.begin(),
			[](auto& arg, auto& desc) { return std::make_pair(arg.getType(), desc); });
	}

	FunctionCallNodeType(const FunctionCallNodeType&) = default;
	FunctionCallNodeType(FunctionCallNodeType&&) = default;

	llvm::Function* function;

	virtual void codegen(size_t execInputID, llvm::Function* f, const std::vector<llvm::Value*>& io,
		llvm::BasicBlock* codegenIntoBlock,
		const std::vector<llvm::BasicBlock*>& outputBlocks) const override
	{
		llvm::IRBuilder<> codegenInto(codegenIntoBlock);
		
		auto ret = codegenInto.CreateCall(function, io);

		// we can optimize with a unconditional jump
		if (execOutputs.size() == 1) {
			codegenInto.CreateBr(outputBlocks[0]);

		} else {
			auto sw = codegenInto.CreateSwitch(ret, outputBlocks[0], execOutputs.size());

			for (size_t i = 0; i < outputBlocks.size(); ++i) {
				sw->addCase(llvm::ConstantInt::get(llvm::IntegerType::get(context->context, 32), i),
					outputBlocks[i]);
			}
		}
	}

	virtual std::unique_ptr<NodeType> clone() const override
	{
		return std::make_unique<FunctionCallNodeType>(*this);
	}
};
}

#endif  // CHIG_NODE_TYPE_HPP
