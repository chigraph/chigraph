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
	IfNodeType(Context& con);

	virtual Result codegen(size_t /*execInputID*/, llvm::Module* mod, llvm::Function*,
		const gsl::span<llvm::Value*> io, llvm::BasicBlock* codegenInto,
		const gsl::span<llvm::BasicBlock*> outputBlocks) const override;

	virtual std::unique_ptr<NodeType> clone() const override;
};

struct EntryNodeType : NodeType {
	EntryNodeType(Context& con, const gsl::span<std::pair<llvm::Type*, std::string>> funInputs);

	// the function doesn't have to do anything...this class just holds metadata
	virtual Result codegen(size_t /*inputExecID*/, llvm::Module* mod, llvm::Function* f,
		const gsl::span<llvm::Value*> io, llvm::BasicBlock* codegenInto,
		const gsl::span<llvm::BasicBlock*> outputBlocks) const override;

	virtual std::unique_ptr<NodeType> clone() const override;

	nlohmann::json toJSON() const override;
};

struct ConstIntNodeType : NodeType {
	ConstIntNodeType(Context& con, int num);

	virtual Result codegen(size_t /*inputExecID*/, llvm::Module* mod, llvm::Function* f,
		const gsl::span<llvm::Value*> io, llvm::BasicBlock* codegenInto,
		const gsl::span<llvm::BasicBlock*> outputBlocks) const override;

	virtual std::unique_ptr<NodeType> clone() const override;

	nlohmann::json toJSON() const override;

	int number;
};

struct ConstBoolNodeType : NodeType {
	ConstBoolNodeType(Context& con, bool num);

	virtual Result codegen(size_t /*inputExecID*/, llvm::Module* mod, llvm::Function* f,
		const gsl::span<llvm::Value*> io, llvm::BasicBlock* codegenInto,
		const gsl::span<llvm::BasicBlock*> outputBlocks) const override;

	virtual std::unique_ptr<NodeType> clone() const override;

	nlohmann::json toJSON() const override;

	bool value;
};

struct ExitNodeType : NodeType {
	ExitNodeType(Context& con, const gsl::span<std::pair<llvm::Type*, std::string>> funOutputs);

	virtual Result codegen(size_t execInputID, llvm::Module* mod, llvm::Function* f,
		const gsl::span<llvm::Value*> io, llvm::BasicBlock* codegenInto,
		const gsl::span<llvm::BasicBlock*> outputBlocks) const override;

	virtual std::unique_ptr<NodeType> clone() const override;

	nlohmann::json toJSON() const override;
};

struct StringLiteralNodeType : NodeType {
	StringLiteralNodeType(Context& con, std::string str);

	virtual Result codegen(size_t execInputID, llvm::Module* mod, llvm::Function* f,
		const gsl::span<llvm::Value*> io, llvm::BasicBlock* codegenInto,
		const gsl::span<llvm::BasicBlock*> outputBlocks) const override;

	virtual std::unique_ptr<NodeType> clone() const override;

	nlohmann::json toJSON() const override;

	std::string literalString;
};

struct LangModule : ChigModule {
	LangModule(Context& context);
	~LangModule() = default;

	virtual Result createNodeType(gsl::cstring_span<> name, const nlohmann::json& json_data,
		std::unique_ptr<NodeType>* toFill) const override;
	virtual llvm::Type* getType(gsl::cstring_span<> name) const override;

	virtual std::vector<std::string> getNodeTypeNames() const override
	{
		return {"if", "entry", "exit", "const-int", "strliteral", "const-bool"};
	}
	virtual std::vector<std::string> getTypeNames() const override
	{
		return {"i32", "i1", "double"};  // TODO: do i need more?
	}

	std::unordered_map<std::string,
		std::function<std::unique_ptr<NodeType>(const nlohmann::json&, Result&)>>
		nodes;
};
}

#endif  // CHIG_LANG_MODULE_HPP
