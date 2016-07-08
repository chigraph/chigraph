#ifndef CHIG_NODE_TYPE_HPP
#define CHIG_NODE_TYPE_HPP

#pragma once

#include "chig/json.hpp"

#include <iterator>
#include <utility>

#include <llvm/IR/Value.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Module.h>


namespace chig {

// generic type
struct NodeType {

	NodeType() {}
	
	virtual ~NodeType() = default;

	std::string name;
	std::string module;
	std::string description;

	// inputs and outputs
	std::vector<std::pair<llvm::Type*, std::string>> inputs;
	
	std::vector<std::pair<llvm::Type*, std::string>> outputs;
	
	unsigned int numOutputExecs = 1;

	virtual void codegen(const std::vector<llvm::Value*>& io, llvm::IRBuilder<>* codegenInto, const std::vector<llvm::BasicBlock*>& outputBlocks) const = 0;
	virtual nlohmann::json toJSON() const {return {};}
	
	virtual std::unique_ptr<NodeType> clone() const = 0;
	
};

struct FunctionCallNodeType : NodeType {

	FunctionCallNodeType(llvm::Module* argModule, llvm::Function* func, int num_inputs, int numExecOutputs, std::string argDescription, const std::vector<std::string>& iodescs) : function{func} {
		
		module = argModule->getName();
		
		numOutputExecs = numExecOutputs;
		
		name = func->getName();
		description = std::move(argDescription);

		// populate inputs and outputs
		inputs.resize(num_inputs);
		auto beginningOfOutptus = func->getArgumentList().begin();
		std::advance(beginningOfOutptus, num_inputs);
		
		std::transform(func->getArgumentList().begin(), beginningOfOutptus, iodescs.begin(), inputs.begin(), 
			[](auto& arg, auto& desc){return std::make_pair(arg.getType(), desc);});

		int num_outputs = std::distance(func->getArgumentList().begin(), func->getArgumentList().end()) - num_inputs;

		outputs.resize(num_outputs);
		std::transform(beginningOfOutptus, func->getArgumentList().end(), iodescs.begin() + num_inputs, outputs.begin(), [](auto& arg, auto& desc){return std::make_pair(arg.getType(), desc);});

	}

	FunctionCallNodeType(const FunctionCallNodeType&) = default;
	FunctionCallNodeType(FunctionCallNodeType&&) = default;
	
	llvm::Function* function;

	virtual void codegen(const std::vector<llvm::Value*>& io, llvm::IRBuilder<>* codegenInto, const std::vector<llvm::BasicBlock*>& outputBlocks) const override {

		auto ret = codegenInto->CreateCall(function, io);
		
		// TODO: better default
		auto sw = codegenInto->CreateSwitch(ret, outputBlocks[0], numOutputExecs); 
		
		for(size_t i = 0; i < outputBlocks.size(); ++i) {
			sw->addCase(llvm::ConstantInt::get(llvm::IntegerType::get(llvm::getGlobalContext(), 32), i), outputBlocks[i]);
		}
		
	}
	
	virtual std::unique_ptr<NodeType> clone() const override {
		return std::make_unique<FunctionCallNodeType>(*this);
	}

};

struct IfNodeType : NodeType {
	
	IfNodeType() {
		
		module = "lang";
		name = "if";
		description = "branch on a bool";
		
		numOutputExecs = 2;
		
		inputs = { {llvm::Type::getInt1Ty(llvm::getGlobalContext()), "condition"} };
		
	}
	
	virtual void codegen(const std::vector<llvm::Value*>& io, llvm::IRBuilder<>* codegenInto, const std::vector<llvm::BasicBlock*>& outputBlocks) const override {

		codegenInto->CreateCondBr(io[0], outputBlocks[0], outputBlocks[1]);
		
	}
	
	virtual std::unique_ptr<NodeType> clone() const override{
		return std::make_unique<IfNodeType>(*this);
	}
	
	
};

struct EntryNodeType : NodeType {
	
	EntryNodeType(const std::vector<std::pair<llvm::Type*, std::string>>& funInputs) {
		
		module = "lang";
		name = "entry";
		description = "entry to a function";
		
		numOutputExecs = 1;
		
		outputs = funInputs;
		
	}

	// this is treated differently during codegen
	virtual void codegen(const std::vector<llvm::Value*>& io, llvm::IRBuilder<>* codegenInto, const std::vector<llvm::BasicBlock*>& outputBlocks) const override {}
	
	virtual std::unique_ptr<NodeType> clone() const override {
		return std::make_unique<EntryNodeType>(*this);
	}
};

}

#endif // CHIG_NODE_TYPE_HPP
