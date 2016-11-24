#ifndef CHIG_JSON_MODULE_HPP
#define CHIG_JSON_MODULE_HPP

#pragma once

#include "chig/Fwd.hpp"
#include "chig/ToString.hpp"
#include "chig/json.hpp"

#include "chig/ChigModule.hpp"
#include "chig/NodeType.hpp"

#include <vector>

namespace chig
{
struct JsonModule : public ChigModule {
	/// Constructor for a json module
	JsonModule(const nlohmann::json& json_data, Context& cont, Result* res);

	JsonModule(const JsonModule&) = delete;
	JsonModule(JsonModule&&) = delete;

	JsonModule& operator=(const JsonModule&) = delete;
	JsonModule& operator=(JsonModule&&) = delete;
	
	Result createNodeType(
		const char* name, const nlohmann::json& json_data,std::unique_ptr<NodeType>* toFill) const override;
	llvm::Type* getType(const char* name) const override {return nullptr;}

	virtual std::vector<std::string> getNodeTypeNames() const override; // TODO: implement
	virtual std::vector<std::string> getTypeNames() const override { return {}; } // TODO: implement
	
	Result toJSON(nlohmann::json* to_fill) const;
	
	std::vector<std::unique_ptr<GraphFunction>> functions;
	
	Result compile(std::unique_ptr<llvm::Module>* mod) const;

	std::vector<std::string> dependencies;
	
	GraphFunction* graphFuncFromName(const char* str) const;
	
};

struct JsonFuncCallNodeType : public NodeType {
	
	JsonFuncCallNodeType(Context* c, const JsonModule* module, const char* funcname);
	
	Result codegen(size_t execInputID, llvm::Module * mod, llvm::Function * f, const std::vector<llvm::Value *> & io, llvm::BasicBlock * codegenInto, const std::vector<llvm::BasicBlock *> & outputBlocks) const override;
	
	nlohmann::json toJSON() const override;
	
	std::unique_ptr<NodeType> clone() const override;
	
	const JsonModule* JModule;
	
};



}

#endif  // CHIG_JSON_MODULE_HPP
