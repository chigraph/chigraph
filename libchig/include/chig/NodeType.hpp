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
	NodeType(ChigModule& mod) : module{&mod}, context{&mod.context()} {}
	virtual ~NodeType() = default;

	std::string name;
	std::string description;

	ChigModule* module;
	Context* context;

	// inputs and outputs
	std::vector<std::pair<DataType, std::string>> dataInputs;

	std::vector<std::pair<DataType, std::string>> dataOutputs;

	std::vector<std::string> execInputs;
	std::vector<std::string> execOutputs;

	std::string getQualifiedName() const { return module->name() + ":" + name; }
	/// A virtual function that is called when this node needs to be called
	/// \param execInputID The ID of the exec input
	/// \param io This has the values that are the inputs and outputs of the function.
	/// This vector will always have the size of `inputs.size() + outputs.size()` and starts with
	/// the inputs.
	/// The types are gaurenteed to be the same as inputs and outputs
	/// \param codegenInto The IRBuilder object that is used to place calls into
	/// \param outputBlocks The blocks that can be outputted. This will be the same size as
	/// numOutputExecs.
	/// \return The Result
	virtual Result codegen(size_t execInputID, llvm::Module* mod, llvm::Function* f,
		const gsl::span<llvm::Value*> io, llvm::BasicBlock* codegenInto,
		const gsl::span<llvm::BasicBlock*> outputBlocks) const = 0;

	/// Create the JSON necessary to store the object.
	/// \ret The json obejct
	virtual nlohmann::json toJSON() const { return {}; }
	/// Clones the type
	/// \return The clone
	virtual std::unique_ptr<NodeType> clone() const = 0;
};
}

#endif  // CHIG_NODE_TYPE_HPP
