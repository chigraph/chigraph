#ifndef CHIG_NODE_TYPE_HPP
#define CHIG_NODE_TYPE_HPP

#pragma once

#include "chig/Context.hpp"
#include "chig/DataType.hpp"
#include "chig/Result.hpp"
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
	NodeType(ChigModule& mod) : mModule{&mod}, mContext{&mod.context()} {}
	virtual ~NodeType() = default;

	std::string qualifiedName() const { return module().name() + ":" + name(); }
	/// A virtual function that is called when this node needs to be called
	/// \param execInputID The ID of the exec input
	/// \param modToCodegenInto The module that is being generated
	/// \param owningModule The module that has been generated from the ChigModule that this node type is a part of
	/// \param f The function that is being generated
	/// \param io This has the values that are the inputs and outputs of the function.
	/// This vector will always have the size of `inputs.size() + outputs.size()` and starts with
	/// the inputs.
	/// The types are gaurenteed to be the same as inputs and outputs
	/// \param codegenInto The IRBuilder object that is used to place calls into
	/// \param outputBlocks The blocks that can be outputted. This will be the same size as
	/// numOutputExecs.
	/// \return The Result
	virtual Result codegen(size_t execInputID, llvm::Module* modToCodegenInto, llvm::Function* f,
		const gsl::span<llvm::Value*> io, llvm::BasicBlock* codegenInto,
		const gsl::span<llvm::BasicBlock*> outputBlocks) const = 0;

	/// Create the JSON necessary to store the object.
	/// \ret The json obejct
	virtual nlohmann::json toJSON() const { return {}; }
	/// Clones the type
	/// \return The clone
	virtual std::unique_ptr<NodeType> clone() const = 0;

	std::string name() const { return mName; }
	std::string description() const { return mDescription; }
	ChigModule& module() { return *mModule; }
	const ChigModule& module() const { return *mModule; }
	Context& context() { return *mContext; }
	const Context& context() const { return *mContext; }
	const std::vector<std::pair<DataType, std::string>>& dataInputs() const { return mDataInputs; }
	const std::vector<std::pair<DataType, std::string>>& dataOutputs() const
	{
		return mDataOutputs;
	}

	const std::vector<std::string>& execInputs() const { return mExecInputs; }
	const std::vector<std::string>& execOutputs() const { return mExecOutputs; }
protected:
	void setName(std::string name) { mName = std::move(name); }
	void setDescription(std::string desc) { mDescription = std::move(desc); }
	void setDataInputs(std::vector<std::pair<DataType, std::string>> newInputs)
	{
		mDataInputs = std::move(newInputs);
	}
	void setDataOutputs(std::vector<std::pair<DataType, std::string>> newOutputs)
	{
		mDataOutputs = std::move(newOutputs);
	}
	void setExecInputs(std::vector<std::string> newInputs) { mExecInputs = std::move(newInputs); }
	void setExecOutputs(std::vector<std::string> newOutputs)
	{
		mExecOutputs = std::move(newOutputs);
	}

private:
	std::string mName, mDescription;
	ChigModule* mModule;
	Context* mContext;

	std::vector<std::pair<DataType, std::string>> mDataInputs;
	std::vector<std::pair<DataType, std::string>> mDataOutputs;

	std::vector<std::string> mExecInputs;
	std::vector<std::string> mExecOutputs;
};
}

#endif  // CHIG_NODE_TYPE_HPP
