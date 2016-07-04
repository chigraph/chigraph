#ifndef CHIG_NODE_TYPE_HPP
#define CHIG_NODE_TYPE_HPP

#pragma once

#include "chig/ImportedModule.hpp"

#include <iterator>
#include <utility>

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

	FunctionCallNodeType(llvm::Function* func, int num_inputs, std::string argDescription, const std::vector<std::string>& iodescs) : function{func} {
		name = func->getName();
		description = std::move(argDescription);

		// populate inputs and outputs
		inputTypes.resize(num_inputs);
		auto beginningOfOutptus = func->getArgumentList().begin();
		std::advance(beginningOfOutptus, num_inputs);
		
		std::transform(func->getArgumentList().begin(), beginningOfOutptus, inputTypes.begin(), [](auto& arg){return arg.getType();});

		int num_outputs = std::distance(func->getArgumentList().begin(), func->getArgumentList().end()) - num_inputs;

		outputTypes.resize(num_outputs);
		std::transform(beginningOfOutptus, func->getArgumentList().end(), outputTypes.begin(), [](auto& arg){return arg.getType();});

	}

	llvm::Function* function;

	virtual void codegen(std::vector<llvm::Value*> inputs, std::vector<llvm::Value*> outputs, llvm::BasicBlock* codegenInto, std::vector<llvm::BasicBlock*> outputBlocks) override {



	}

};

}

#endif // CHIG_NODE_TYPE_HPP
