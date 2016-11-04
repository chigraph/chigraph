#ifndef CHIG_LANG_MODULE_HPP
#define CHIG_LANG_MODULE_HPP

#pragma once

#include "chig/ChigModule.hpp"
#include "chig/Context.hpp"
#include "chig/NodeType.hpp"

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

namespace chig
{
struct IfNodeType : NodeType {
	IfNodeType(Context& con) : NodeType(con)
	{
		module = "lang";
		name = "if";
		description = "branch on a bool";

		execInputs = {""};
		execOutputs = {"True", "False"};

		dataInputs = {{llvm::Type::getInt1Ty(context->context), "condition"}};
	}

	virtual void codegen(size_t /*execInputID*/, llvm::Function*, const std::vector<llvm::Value*>& io,
		llvm::BasicBlock* codegenInto,
		const std::vector<llvm::BasicBlock*>& outputBlocks) const override
	{
		llvm::IRBuilder<> builder(codegenInto);
		builder.CreateCondBr(io[0], outputBlocks[0], outputBlocks[1]);
	}

	virtual std::unique_ptr<NodeType> clone() const override
	{
		return std::make_unique<IfNodeType>(*this);
	}
};

struct EntryNodeType : NodeType {
	EntryNodeType(Context& con, const std::vector<std::pair<llvm::Type*, std::string>>& funInputs)
		: NodeType{con}
	{
		module = "lang";
		name = "entry";
		description = "entry to a function";

		execOutputs = {""};

		dataOutputs = funInputs;
	}

	// the function doesn't have to do anything...this class just holds metadata
	virtual void codegen(size_t /*inputExecID*/, llvm::Function* f, const std::vector<llvm::Value*>& io, llvm::BasicBlock* codegenInto, const std::vector<llvm::BasicBlock*>& outputBlocks) const override {
		llvm::IRBuilder<> builder(codegenInto); 
		// just go to the block
		builder.CreateBr(outputBlocks[0]);
		
	}

	virtual std::unique_ptr<NodeType> clone() const override
	{
		return std::make_unique<EntryNodeType>(*this);
	}

	nlohmann::json toJSON() const override
	{
		nlohmann::json ret;

		for (auto& pair : dataOutputs) {
			// TODO: user made types
			ret[pair.second] = "lang:" + context->stringifyType(pair.first);
		}

		return ret;
	}
};

struct ConstIntNodeType : NodeType {
	
	ConstIntNodeType(Context& con, int num)
		: NodeType{con},
		number{num}
	{
		module = "lang";
		name = "const-int";
		description = "constant int value";

		execInputs = {""};
		execOutputs = {""};

		dataOutputs = {{llvm::IntegerType::getInt32Ty(con.context), "out"}};
		
	}

	// the function doesn't have to do anything...this class just holds metadata
	virtual void codegen(size_t /*inputExecID*/, llvm::Function* f, const std::vector<llvm::Value*>& io, llvm::BasicBlock* codegenInto, const std::vector<llvm::BasicBlock*>& outputBlocks) const override {
		llvm::IRBuilder<> builder(codegenInto); 
		// just go to the block
		assert(io.size() == 1);
		
		builder.CreateStore(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(context->context), number), io[0], false);
		builder.CreateBr(outputBlocks[0]);
		
	}

	virtual std::unique_ptr<NodeType> clone() const override
	{
		return std::make_unique<ConstIntNodeType>(*this);
	}

	nlohmann::json toJSON() const override
	{
		nlohmann::json ret = number;

		return ret;
	}
	
	int number;
};

struct ExitNodeType : NodeType {
	ExitNodeType(Context& con, const std::vector<std::pair<llvm::Type*, std::string>>& funOutputs)
		: NodeType{con}
	{
		execInputs = {""};

		module = "lang";
		name = "exit";
		description = "exit from a function; think return";

		dataInputs = funOutputs;
	}

	virtual void codegen(size_t execInputID, llvm::Function* f, const std::vector<llvm::Value*>& io,
		llvm::BasicBlock* codegenInto, const std::vector<llvm::BasicBlock*>&) const override
	{
		// assign the return types
		llvm::IRBuilder<> builder(codegenInto);
		size_t ret_start = f->arg_size() - io.size(); // returns are after args, find where returns start
		auto arg_iter = f->arg_begin();
		std::advance(arg_iter, ret_start);
		for(int idx = 0; idx < io.size(); ++idx) {
			builder.CreateStore(io[idx], &*arg_iter, false); // TODO: volitility?
			++arg_iter;
		}
		
		builder.CreateRet(llvm::ConstantInt::get(llvm::Type::getInt32Ty(context->context), execInputID));
	}

	virtual std::unique_ptr<NodeType> clone() const override
	{
		return std::make_unique<ExitNodeType>(*this);
	}

	nlohmann::json toJSON() const override
	{
		nlohmann::json ret;

		for (auto& pair : dataOutputs) {
			// TODO: user made types
			ret[pair.second] = "lang:" + context->stringifyType(pair.first);
		}

		return ret;
	}
};

struct LangModule : ChigModule {
	LangModule(Context& context);

	std::unique_ptr<NodeType> createNodeType(
		const char* name, const nlohmann::json& json_data) const override;
	llvm::Type* getType(const char* name) override;

	std::unordered_map<std::string, std::function<std::unique_ptr<NodeType>(const nlohmann::json&)>>
		nodes;
};

}

#endif  // CHIG_LANG_MODULE_HPP
