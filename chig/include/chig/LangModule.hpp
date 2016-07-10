#ifndef CHIG_LANG_MODULE_HPP
#define CHIG_LANG_MODULE_HPP

#pragma once

#include "chig/ChigModule.hpp"
#include "chig/NodeType.hpp"
#include "chig/Context.hpp"

#include <memory>
#include <functional>
#include <unordered_map>
#include <string>

namespace chig {


struct IfNodeType : NodeType {
	
	IfNodeType(Context& con) : NodeType(con) {
		
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
	
	EntryNodeType(Context& con, const std::vector<std::pair<llvm::Type*, std::string>>& funInputs) : NodeType{con} {
		
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
	
	nlohmann::json toJSON() const override {
		nlohmann::json ret;
		
		
		
		for(auto& pair : outputs) {
			// TODO: user made types
			ret[pair.second] = "lang:" + context->stringifyType(pair.first);
		}
		
		return ret;
	}
};


struct LangModule : ChigModule {
	
	LangModule(Context& context);
	
	std::unique_ptr<NodeType> createNodeType(const char * name, const nlohmann::json & json_data) const override;
	llvm::Type* getType(const char* name) override;
	
	std::unordered_map<std::string, std::function<std::unique_ptr<NodeType> (const nlohmann::json &)>> nodes;
	
};

}

#endif // CHIG_LANG_MODULE_HPP
