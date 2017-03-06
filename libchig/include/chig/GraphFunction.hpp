/// \file chig/GraphFunction.hpp
/// Declares the GraphFunction class

#ifndef CHIG_GRAPH_FUNCTION_HPP
#define CHIG_GRAPH_FUNCTION_HPP

#pragma once

#include "chig/Fwd.hpp"
#include "chig/json.hpp"
#include "chig/HashUuid.hpp"

#include <gsl/gsl>

#include <unordered_map>

#include <llvm/IR/DerivedTypes.h>  // for FunctionType

#include <boost/filesystem.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>

namespace chig {
/// this is an AST-like representation of a function in a graph
/// It is used for IDE-like behavior, codegen, and JSON generation.
struct GraphFunction {
	/// Construct a graph--don't call this directly use GraphModule::getorCreateFunction
	/// \param mod The owning module
	/// \param name The name of the function
	/// \param dataIns The data inputs to the function
	/// \param dataOuts The data outputs of the function
	/// \param execIns The exec inputs to the function
	/// \param execOuts The exec outputs to the function
	GraphFunction(GraphModule& mod, gsl::cstring_span<> name, std::vector<NamedDataType> dataIns,
	              std::vector<NamedDataType> dataOuts, std::vector<std::string> execIns,
	              std::vector<std::string> execOuts);

	GraphFunction(const GraphFunction&) = delete;
	GraphFunction(GraphFunction&&)      = delete;

	GraphFunction& operator=(const GraphFunction&) = delete;
	GraphFunction& operator=(GraphFunction&&) = delete;

	/// Destructor
	~GraphFunction() = default;

	/// \name Node Manipulation
	/// Functions for mainpulating nodes; getting, adding
	/// \{

	/// Get the nodes in the function
	/// Usually called by connectData or connectExec or GraphFunction
	/// \return The nodes, mapped by id, value
	std::unordered_map<boost::uuids::uuid, std::unique_ptr<NodeInstance>>& nodes() {
		return mNodes;
	}
	/// \copydoc Graph::nodes
	const std::unordered_map<boost::uuids::uuid, std::unique_ptr<NodeInstance>>& nodes() const {
		return mNodes;
	}

	/// Gets the node with type lang:entry
	/// returns nullptr on failure
	/// Also returns nullptr if there are two entry nodes, which is illegal
	/// \return the entry node
	NodeInstance* entryNode() const noexcept;

	/// Add a node to the graph
	/// \param type The type of the node
	/// \param x The x location of the node
	/// \param y The y location of the node
	/// \param id The node ID
	/// \retval toFill The nodeInstance to fill to, optional.
	/// \return The result
	Result insertNode(std::unique_ptr<NodeType> type, float x, float y,
	                  boost::uuids::uuid id     = boost::uuids::random_generator()(),
	                  NodeInstance**     toFill = nullptr);

	/// Gets the nodes with a given type
	/// \param module The module the type is in
	/// \param name The name of the type
	/// \return A vector of NodeInstance
	std::vector<NodeInstance*> nodesWithType(const boost::filesystem::path& module,
	                                         gsl::cstring_span<>            name) const noexcept;

	/// Add a node to the graph using module, type, and json
	/// \param moduleName The name of the module that typeName is in
	/// \param typeName The name of the node type in the module with the name moduleName
	/// \param typeJSON The JSON to be passed to create the type
	/// \param x The x location of the node
	/// \param y The y location of the node
	/// \param id The node ID
	/// \retval toFill The NodeInstance* to fill to, optional
	/// \return The Result
	Result insertNode(const boost::filesystem::path& moduleName, gsl::cstring_span<> typeName,
	                  const nlohmann::json& typeJSON, float x, float y,
	                  boost::uuids::uuid id     = boost::uuids::random_generator()(),
	                  NodeInstance**     toFill = nullptr);

	/// Remove a node from the function. Also disconnect it's connections.
	/// \param nodeToRemove The node to remove
	/// \return The result
	Result removeNode(NodeInstance* nodeToRemove);

	/// Creates an entry node if it doesn't already exist, else just return it
	/// \param x The x coordinate of the new entry, or changes the existing entry node to be at this
	/// X location
	/// \param y The y coordinate of the new entry, or changes the existing entry node to be at this
	/// Y location
	/// \param id The ID of the node, disregarded if there is already an entry
	/// \retval toFill The NodeInstance* to fill, optional
	/// \return The Result
	Result getOrInsertEntryNode(float x, float y,
	                            boost::uuids::uuid id     = boost::uuids::random_generator()(),
	                            NodeInstance**     toFill = nullptr);

	/// \}

	/// Create a fresh NodeType for an entry
	/// \retval toFill The NodeType pointer to fill
	/// \pre toFill isn't null (the value the unique_ptr points to be can be null, but not the
	/// pointer to the unique_ptr)
	/// \return The result
	Result createEntryNodeType(std::unique_ptr<NodeType>* toFill) const;

	/// Create a fresh NodeType for an exit
	/// \retval toFill The NodeType pointer to fill
	/// \pre toFill isn't null (the value the unique_ptr points to be can be null, but not the
	/// pointer to the unique_ptr)
	/// \return The result
	Result createExitNodeType(std::unique_ptr<NodeType>* toFill) const;

	/// Get the LLVM function type for the function
	/// \return The function type
	llvm::FunctionType* functionType() const;

	// TODO: check uses and replace to avoid errors
	/// \name Data input modifiers
	/// \{

	/// Get the function data inputs in the format {type, docstring}
	/// \return The inputs
	const std::vector<NamedDataType>& dataInputs() const { return mDataInputs; }
	/// Add an input to the end of the argument list
	/// \param type The new input type
	/// \param name The name of the input (just for documentation)
	/// \param addBefore The input ID to add before, default is end
	void addDataInput(const DataType& type, gsl::cstring_span<> name,
	                  size_t addBefore = (std::numeric_limits<size_t>::max)());

	/// Remove an input from the argument list
	/// Also removes invalid connections
	/// If idx is out of range, this function does nothing.
	/// \param idx The index to delete
	void removeDataInput(size_t idx);

	/// Rename a data input
	/// This also updates the entry node
	/// If idx is out of range, this function does nothing.
	/// \param idx The index to rename
	/// \param newName The new name
	void renameDataInput(size_t idx, std::string newName);

	/// Change the type of a data input
	/// This also updates the entry node and disconnects invalid connections.
	/// If idx is out of range, this function does nothing.
	/// \param idx The index to retype
	/// \param newType The new type
	void retypeDataInput(size_t idx, DataType newType);

	/// \}

	/// \name Data output modifiers
	/// \{

	/// Get the function data outputs in the format {type, docstring}
	/// \return The outputs
	const std::vector<NamedDataType>& dataOutputs() const { return mDataOutputs; }
	/// Add an data output to the end of the argument list
	/// \param type The new output type
	/// \param name The name of the output (just for documentation)
	/// \param addBefore The output to add before, default is end
	void addDataOutput(const DataType& type, gsl::cstring_span<> name,
	                   size_t addBefore = (std::numeric_limits<size_t>::max)());

	/// Remove an data output from the argument list
	/// Also removes invalid connections
	/// If idx is out of range, this function does nothing.
	/// \param idx The index to delete
	void removeDataOutput(size_t idx);
	/// Modify an data output (change it's type and docstring)

	/// Rename a data output
	/// This also updates all exit nodes
	/// If idx is out of range, this function does nothing.
	/// \param idx The index to rename
	/// \param newName The new name
	void renameDataOutput(size_t idx, std::string newName);

	/// Change the type of a data output
	/// This also updates all exit nodes and disconnects invalid connections.
	/// If idx is out of range, this function does nothing.
	/// \param idx The index to retype
	/// \param newType The new type
	void retypeDataOutput(size_t idx, DataType newType);

	/// \}

	/// \name Exec input modifiers
	/// \{

	/// Get the function exec inputs
	/// \return The exec outputs
	const std::vector<std::string>& execInputs() const { return mExecInputs; }
	/// Add an exec input to the end of the argument list
	/// \param name The name of the input (just for documentation)
	/// \param addBefore the input to add after
	void addExecInput(gsl::cstring_span<> name,
	                  size_t              addBefore = (std::numeric_limits<size_t>::max)());

	/// Remove an exec input from the argument list
	/// If idx is out of range, this function does nothing.
	/// \param idx The index to delete
	void removeExecInput(size_t idx);

	/// Change the name for an exec input
	/// If idx is out of range, this function does nothing.
	/// \param idx The index to change
	/// \param name The new name.
	void renameExecInput(size_t idx, std::string name);

	/// \}

	/// \name Exec output modifiers
	/// \{
	/// Getters/modifers for exec output

	/// Get the function exec outputs
	/// \return The exec outputs
	const std::vector<std::string>& execOutputs() const { return mExecOutputs; }
	/// Add an exec output to the end of the argument list
	/// \param name The name of the output (just for documentation)
	/// \param addBefore The output to add after
	void addExecOutput(gsl::cstring_span<> name,
	                   size_t              addBefore = (std::numeric_limits<size_t>::max)());

	/// Remove an exec output from the argument list
	/// If idx is out of range, this function does nothing.
	/// \param idx The index to delete
	void removeExecOutput(size_t idx);

	/// Rename an exec output
	/// If idx is out of range, this function does nothing.
	/// \param idx The index to change
	/// \param name The new name.
	void renameExecOutput(size_t idx, std::string name);

	/// \}

	/// \name Local Variable Manipulation functions
	/// \{

	/// Get the local variables
	/// \return The local variables
	const std::vector<NamedDataType>& localVariables() const { return mLocalVariables; }

	/// Get a local varaible by name
	/// \param name The name of the variable
	/// \return The local or {} if not found
	NamedDataType localVariableFromName(gsl::cstring_span<> name) const;

	/// Create a new local varaible in the module
	/// \param name The name of the local variable
	/// \param type The type of the local
	/// \param inserted True if the local varaiable was new, optional
	/// \return The new local variable or the already existing one
	NamedDataType getOrCreateLocalVariable(std::string name, DataType type,
	                                       bool* inserted = nullptr);

	/// Remove a local variable from the function by name
	/// \param name The name of the local variable to remove
	/// \return True if a local was actually removed, false if no local by that name existed
	bool removeLocalVariable(gsl::cstring_span<> name);

	/// Rename a local variable
	/// \param oldName The name of the existing local to change
	/// \param newName The new name of the local
	void renameLocalVariable(std::string oldName, std::string newName);

	/// Set a new type to a local variable
	/// \param name The name of the local to change
	/// \param newType The new type
	void retypeLocalVariable(gsl::cstring_span<> name, DataType newType);

	/// \}

	/// Set the description of the function
	/// \param newDesc The new description
	void setDescription(std::string newDesc) { mDescription = std::move(newDesc); }

	/// Get the description of the function
	/// \return The description
	const std::string& description() const { return mDescription; };

	// Various getters
	//////////////////

	/// Get the context
	/// \return The context
	Context& context() const { return *mContext; }
	/// Get the name of the function
	/// \return The name of the function
	std::string name() const { return mName; }
	/// Get the GraphModule that contains this GraphFunction
	/// \return The GraphModule.
	GraphModule& module() const { return *mModule; }

private:
	void updateEntries();  // update the entry node to work with
	void updateExits();

	GraphModule* mModule;
	Context*     mContext;
	std::string  mName;  /// the name of the function
	std::string  mDescription;

	std::vector<NamedDataType> mDataInputs;
	std::vector<NamedDataType> mDataOutputs;

	std::vector<std::string> mExecInputs;
	std::vector<std::string> mExecOutputs;

	std::vector<NamedDataType> mLocalVariables;

	std::unordered_map<boost::uuids::uuid, std::unique_ptr<NodeInstance>>
	    mNodes;  /// Storage for the nodes
};

/// Parse a colonated pair
/// Example: lang:i32 would turn into {lang, i32}
/// \param in The colonated pair
/// \return {first, second}
inline std::pair<std::string, std::string> parseColonPair(const std::string& in) {
	size_t colonID = in.find(':');
	if (colonID == std::string::npos) { return {}; }
	std::string module = in.substr(0, in.find(':'));
	std::string name   = in.substr(in.find(':') + 1);

	return {module, name};
}
}  // namespace chig

#endif  // CHIG_GRAPH_FUNCTION_HPP
