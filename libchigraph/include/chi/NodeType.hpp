/// \file chi/NodeType.hpp
/// Defines the NodeType class

#ifndef CHI_NODE_TYPE_HPP
#define CHI_NODE_TYPE_HPP

#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "chi/Fwd.hpp"
#include "chi/json.hpp"

namespace chi {
/// A generic node type. All user made types are of JsonNo  deType type, which is defined in
/// JsonModule.cpp. This allows for easy extension of the language.
struct NodeType {
private:
	friend NodeInstance;

public:
	/// Constructor
	/// \param mod The module to create the NodeType in
	/// \param name The name of the NodeType
	/// \param description The description of the NodeType
	NodeType(ChiModule& mod, std::string name = {}, std::string description = {});
	/// Destructor
	virtual ~NodeType();

	/// Get the qualified name of the node type, like module.name():name()
	/// \return The qualified name
	std::string qualifiedName() const;
	/// A virtual function that is called when this node needs to be called
	/// \param execInputID The ID of the exec input
	/// \param nodeLocation The location of the node
	/// \param io This has the values that are the inputs and outputs of the function.
	/// This vector will always have the size of `inputs.size() + outputs.size()` and starts with
	/// the inputs.
	/// The types are gaurenteed to be the same as inputs and outputs
	/// \param codegenInto The BasicBlock object that is used to place calls into
	/// \param outputBlocks The blocks that can be outputted. This will be the same size as
	/// \param compileCache Cache for use of nodes
	/// numOutputExecs.
	/// \return The Result
	virtual Result codegen(
	    size_t execInputID, const llvm::DebugLoc& nodeLocation, const std::vector<llvm::Value*>& io,
	    llvm::BasicBlock* codegenInto, const std::vector<llvm::BasicBlock*>& outputBlocks,
	    std::unordered_map<std::string, std::shared_ptr<void>>&              compileCache) = 0;

	/// Create the JSON necessary to store the object.
	/// \return The json obejct
	virtual nlohmann::json toJSON() const { return {}; }
	/// Clones the type
	/// \return The clone
	virtual std::unique_ptr<NodeType> clone() const = 0;

	/// Get the name of the NodeType in the ChiModule.
	/// \return The name
	std::string name() const { return mName; }
	/// Get the description of the NodeType
	/// \return The description
	std::string description() const { return mDescription; }
	/// Get the ChiModule this NodeType belongs to
	/// \return The ChiModule
	ChiModule& module() const { return *mModule; }
	/// Get the Context this NodeType belongs to
	/// \return The Context
	Context& context() const { return *mContext; }
	/// Get the data inputs for the node
	/// \return The data inputs in the format of {{DataType, description}, ...}
	const std::vector<NamedDataType>& dataInputs() const { return mDataInputs; }
	/// Get the data outputs for the node
	/// \return The data outputs in the format of {{DataType, description}, ...}
	const std::vector<NamedDataType>& dataOutputs() const { return mDataOutputs; }
	/// Get the execution inputs for the node
	/// \return The names of the inputs. The size of this vector is the size of inputs.
	const std::vector<std::string>& execInputs() const { return mExecInputs; }
	/// Get the execution outputs for the node
	/// \return The names of the outputs. The size is the input count.
	const std::vector<std::string>& execOutputs() const { return mExecOutputs; }

	/// Get if this node is pure
	/// \return If it's pure
	bool pure() { return mPure; }

protected:
	/// Set the data inputs for the NodeType
	/// \param newInputs The new inputs
	void setDataInputs(std::vector<NamedDataType> newInputs);
	/// Set the data outputs for the NodeType
	/// \param newOutputs The new outputs
	void setDataOutputs(std::vector<NamedDataType> newOutputs);
	/// Set the exec inputs for the NodeType
	/// \param newInputs The new inputs
	void setExecInputs(std::vector<std::string> newInputs);
	/// Set the exec outputs for the NodeType
	/// \param newOutputs The new outputs
	void setExecOutputs(std::vector<std::string> newOutputs);

	/// Set the name of the type
	/// \param newName The new name
	void setName(std::string newName);

	/// Set the description of the node
	/// \param newDesc The new description
	void setDescription(std::string newDesc);

	/// Make this node pure
	/// For more info on what this means, see https://en.wikipedia.org/wiki/Pure_function
	/// Also, UE4 implements it
	/// https://docs.unrealengine.com/latest/INT/Engine/Blueprints/UserGuide/Functions/#purevsstopimpure
	/// Pure nodes only have no inexecs and no outexecs
	/// When they are called they are backpropagated and all called
	/// They should usually be cheap and sideaffectless
	void makePure();

	/// Get the node instance
	/// \return the node instance
	NodeInstance* nodeInstance() const;

private:
	ChiModule*  mModule;
	Context*    mContext;
	std::string mName, mDescription;

	NodeInstance* mNodeInstance = nullptr;

	std::vector<NamedDataType> mDataInputs;
	std::vector<NamedDataType> mDataOutputs;

	std::vector<std::string> mExecInputs;
	std::vector<std::string> mExecOutputs;

	bool mPure = false;
};
}  // namespace chi

#endif  // CHI_NODE_TYPE_HPP
