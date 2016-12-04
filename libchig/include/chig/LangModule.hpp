#ifndef CHIG_LANG_MODULE_HPP
#define CHIG_LANG_MODULE_HPP

#pragma once

#include "chig/ChigModule.hpp"
#include "chig/Context.hpp"
#include "chig/DataType.hpp"
#include "chig/NodeType.hpp"

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

namespace chig
{
struct IfNodeType : NodeType {
	IfNodeType(LangModule& con);

	virtual Result codegen(size_t /*execInputID*/, llvm::Module* mod, llvm::Function*,
		const gsl::span<llvm::Value*> io, llvm::BasicBlock* codegenInto,
		const gsl::span<llvm::BasicBlock*> outputBlocks) const override;

	virtual std::unique_ptr<NodeType> clone() const override;
};

struct EntryNodeType : NodeType {
	EntryNodeType(LangModule& con, const gsl::span<std::pair<DataType, std::string>> funInputs);

	// the function doesn't have to do anything...this class just holds metadata
	virtual Result codegen(size_t /*inputExecID*/, llvm::Module* mod, llvm::Function* f,
		const gsl::span<llvm::Value*> io, llvm::BasicBlock* codegenInto,
		const gsl::span<llvm::BasicBlock*> outputBlocks) const override;

	virtual std::unique_ptr<NodeType> clone() const override;

	nlohmann::json toJSON() const override;
};

struct ConstIntNodeType : NodeType {
	ConstIntNodeType(LangModule& con, int num);

	virtual Result codegen(size_t /*inputExecID*/, llvm::Module* mod, llvm::Function* f,
		const gsl::span<llvm::Value*> io, llvm::BasicBlock* codegenInto,
		const gsl::span<llvm::BasicBlock*> outputBlocks) const override;

	virtual std::unique_ptr<NodeType> clone() const override;

	nlohmann::json toJSON() const override;

	int number;
};

struct ConstBoolNodeType : NodeType {
	ConstBoolNodeType(LangModule& con, bool num);

	virtual Result codegen(size_t /*inputExecID*/, llvm::Module* mod, llvm::Function* f,
		const gsl::span<llvm::Value*> io, llvm::BasicBlock* codegenInto,
		const gsl::span<llvm::BasicBlock*> outputBlocks) const override;

	virtual std::unique_ptr<NodeType> clone() const override;

	nlohmann::json toJSON() const override;

	bool value;
};

struct ExitNodeType : NodeType {
	ExitNodeType(LangModule& con, const gsl::span<std::pair<DataType, std::string>> funOutputs);

	virtual Result codegen(size_t execInputID, llvm::Module* mod, llvm::Function* f,
		const gsl::span<llvm::Value*> io, llvm::BasicBlock* codegenInto,
		const gsl::span<llvm::BasicBlock*> outputBlocks) const override;

	virtual std::unique_ptr<NodeType> clone() const override;

	nlohmann::json toJSON() const override;
};

struct StringLiteralNodeType : NodeType {
	StringLiteralNodeType(LangModule& con, std::string str);

	virtual Result codegen(size_t execInputID, llvm::Module* mod, llvm::Function* f,
		const gsl::span<llvm::Value*> io, llvm::BasicBlock* codegenInto,
		const gsl::span<llvm::BasicBlock*> outputBlocks) const override;

	virtual std::unique_ptr<NodeType> clone() const override;

	nlohmann::json toJSON() const override;

	std::string literalString;
};

struct LangModule : ChigModule {
	LangModule(Context& ctx);
	~LangModule() = default;

	virtual Result nodeTypeFromName(gsl::cstring_span<> name, const nlohmann::json& json_data,
		std::unique_ptr<NodeType>* toFill) override;
	virtual DataType typeFromName(gsl::cstring_span<> name) override;

	virtual std::vector<std::string> nodeTypeNames() const override
	{
		return {"if", "entry", "exit", "const-int", "strliteral", "const-bool"};
	}
	virtual std::vector<std::string> typeNames() const override
	{
		return {"i32", "i1", "double"};  // TODO: do i need more?
	}
	Result generateModule(std::unique_ptr<llvm::Module>* module) override
	{
		*module = std::make_unique<llvm::Module>("lang", context().llvmContext());

		return {};
	};

	std::unordered_map<std::string,
		std::function<std::unique_ptr<NodeType>(const nlohmann::json&, Result&)>>
		nodes;
};
}

#endif  // CHIG_LANG_MODULE_HPP
