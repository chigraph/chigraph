#ifndef CHIG_NODE_TYPE_HPP
#define CHIG_NODE_TYPE_HPP

#pragma once

#include "chig/ImportedModule.hpp"

#include <iterator>
#include <utility>

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Function.h>


namespace chig {

// generic type
struct NodeType {

	virtual ~NodeType() = default;

	std::string name;
	std::string description;

	// inputs and outputs
	std::vector<llvm::Type*> inputTypes;
	std::vector<llvm::Type*> outputTypes;

	unsigned int numOutputExecs = 1;

	virtual void codegen(std::vector<llvm::Value*> inputs, std::vector<llvm::Value*> outputs, llvm::BasicBlock* codegenInto, std::vector<llvm::BasicBlock*> outputBlocks) = 0;
	
};

struct FunctionCallNodeType : NodeType {

	FunctionCallNodeType(llvm::Function* func, int num_inputs, std::string description, const std::vector<std::string>& iodescs) : function{func} {
		name = func->getName();
		description = std::move(description);

		// populate inputs and outputs
		inputTypes.resize(num_inputs);
		std::copy(func->getArgumentList().begin(), std::advance(func->getArgumentList().begin(), num_inputs), inputTypes.begin());

		int num_outputs = std::distance(func->getArgumentList().begin(), func->getArgumentList().end()) - num_inputs;

		outputTypes.resize(num_outputs);
		std::copy(std::advance(func->getArgumentList().begin(), num_inputs), func->getArgumentList().end(), outputTypes.begin());

	}

	llvm::Function* function;

	virtual void codegen(std::vector<llvm::Value*> inputs, std::vector<llvm::Value*> outputs, llvm::BasicBlock* codegenInto, std::vector<llvm::BasicBlock*> outputBlocks) override {



	}

};

}

#endif // CHIG_NODE_TYPE_HPP
