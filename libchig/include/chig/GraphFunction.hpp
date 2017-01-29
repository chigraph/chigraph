/// \file chig/GraphFunction.hpp
/// Declares the GraphFunction class


#ifndef CHIG_GRAPH_FUNCTION_HPP
#define CHIG_GRAPH_FUNCTION_HPP

#pragma once

#include "chig/Fwd.hpp"
#include "chig/json.hpp"

#include <gsl/gsl>

#include <unordered_map>

#include <llvm/IR/DerivedTypes.h> // for FunctionType

#include <boost/optional.hpp>

namespace chig {
/// this is an AST-like representation of a function in a graph
/// It is used for IDE-like behavior, codegen, and JSON generation.
struct GraphFunction {
	/// Construct a graph
	/// \param mod The owning module
	/// \param name The name of the function
	/// \param dataIns The data inputs to the function
	/// \param dataOuts The data outputs of the function
	/// \param execIns The exec inputs to the function
	/// \param execOuts The exec outputs to the function
	GraphFunction(JsonModule& mod, gsl::cstring_span<>          name,
	              std::vector<std::pair<DataType, std::string>> dataIns,
	              std::vector<std::pair<DataType, std::string>> dataOuts,
	              std::vector<std::string> execIns, std::vector<std::string> execOuts);

	/// Destructor
	~GraphFunction();

	/// \name Node Manipulation
	/// Functions for mainpulating nodes; getting, adding
	/// \{

	/// Get the nodes in the function
	/// Usually called by connectData or connectExec or GraphFunction
	/// \return The nodes, mapped by id, value
	std::unordered_map<std::string, std::unique_ptr<NodeInstance>>& nodes() { return mNodes; }
	/// \copydoc Graph::nodes
	const std::unordered_map<std::string, std::unique_ptr<NodeInstance>>& nodes() const {
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
	/// \param toFill The nodeInstance to fill to, optional.
	/// \return The result
	Result insertNode(std::unique_ptr<NodeType> type, float x, float y, gsl::cstring_span<> id,
	                  NodeInstance** toFill = nullptr);
	
	
	/// Gets the nodes with a given type
	/// \param module The module the type is in
	/// \param name The name of the type
	/// \return A vector of NodeInstance
	std::vector<NodeInstance*> nodesWithType(gsl::cstring_span<> module,
	                                         gsl::cstring_span<> name) const noexcept;


	/// Add a node to the graph using module, type, and json
	/// \param moduleName The name of the module that typeName is in
	/// \param typeName The name of the node type in the module with the name moduleName
	/// \param typeJSON The JSON to be passed to create the type
	/// \param x The x location of the node
	/// \param y The y location of the node
	/// \param id The node ID
	/// \param toFill The NodeInstance to fill to, optional
	/// \return The Result
	Result insertNode(gsl::cstring_span<> moduleName, gsl::cstring_span<> typeName,
	                  const nlohmann::json& typeJSON, float x, float y, gsl::cstring_span<> id,
	                  NodeInstance** toFill = nullptr);

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
	/// \param toFill The NodeInstance* to fill, optional
	/// \return The Result
	Result getOrInsertEntryNode(float x, float y, gsl::cstring_span<> id,
	                            NodeInstance** toFill = nullptr);

	/// \}

	/// Create a fresh NodeType for an entry
	/// \param toFill The NodeType pointer to fill
	/// \return The result
	Result createEntryNodeType(std::unique_ptr<NodeType>* toFill) const;

	/// Create a fresh NodeType for an exit
	/// \param toFill The NodeType pointer to fill
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
	const std::vector<std::pair<DataType, std::string>>& dataInputs() const { return mDataInputs; }
	/// Add an input to the end of the argument list
	/// \param type The new input type
	/// \param name The name of the input (just for documentation)
	/// \param addAfter The input ID to add after
	void addDataInput(const DataType& type, gsl::cstring_span<> name, int addAfter);

	/// Remove an input from the argument list
	/// \param idx The index to delete
	void removeDataInput(int idx);
	/// Modify an input (change it's type and docstring)
	/// \param idx The index to change
	/// \param type The new type. Use {} to keep it's current type
	/// \param name The new name. Use {} to keep it's current name
	void modifyDataInput(int idx, const DataType& type, boost::optional<gsl::cstring_span<>> name);

	/// \}

	/// \name Data output modifiers
	/// \{

	/// Get the function data outputs in the format {type, docstring}
	/// \return The outputs
	const std::vector<std::pair<DataType, std::string>>& dataOutputs() const {
		return mDataOutputs;
	}
	/// Add an data output to the end of the argument list
	/// \param type The new output type
	/// \param name The name of the output (just for documentation)
	/// \param addAfter The output to add after
	void addDataOutput(const DataType& type, gsl::cstring_span<> name, int addAfter);

	/// Remove an data output from the argument list
	/// \param idx The index to delete
	void removeDataOutput(int idx);
	/// Modify an data output (change it's type and docstring)
	/// \param idx The index to change
	/// \param type The new type. Use {} to keep it's current type
	/// \param name The new name. Use {} to keep it's current name
	void modifyDataOutput(int idx, const DataType& type, boost::optional<gsl::cstring_span<>> name);

	/// \}

	/// \name Exec input modifiers
	/// \{

	/// Get the function exec inputs
	/// \return The exec outputs
	const std::vector<std::string>& execInputs() const { return mExecInputs; }
	/// Add an exec input to the end of the argument list
	/// \param name The name of the input (just for documentation)
	/// \param addAfter the input to add after
	void addExecInput(gsl::cstring_span<> name, int addAfter);

	/// Remove an exec input from the argument list
	/// \param idx The index to delete
	void removeExecInput(int idx);

	/// Modify an exec input (change docstring)
	/// \param idx The index to change
	/// \param name The new name.
	void modifyExecInput(int idx, gsl::cstring_span<> name);

	/// \}

	/// \name Exec output modifiers
	/// \{
	/// Getters/modifers for exec output

	/// Get the function exec outputs
	/// \return The exec outputs
	const std::vector<std::string>& execOutputs() const { return mExecOutputs; }
	/// Add an exec output to the end of the argument list
	/// \param name The name of the output (just for documentation)
	/// \param addAfter The output to add after
	void addExecOutput(gsl::cstring_span<> name, int addAfter);

	/// Remove an exec output from the argument list
	/// \param idx The index to delete
	void removeExecOutput(int idx);

	/// Modify an exec output (change docstring)
	/// \param idx The index to change
	/// \param name The new name.
	void modifyExecOutput(int idx, gsl::cstring_span<> name);

	/// \}

	// Various getters
	//////////////////

	/// Get the context
	/// \return The context
	Context& context() const { return *mContext; }
	/// Get the name of the function
	/// \return The name of the function
	std::string name() const { return mName; }
	/// Get the JsonModule that contains this GraphFunction
	/// \return The JsonModule.
	JsonModule& module() const { return *mModule; }

private:
	void updateEntries();  // update the entry node to work with
	void updateExits();

	JsonModule* mModule;
	Context*    mContext;
	std::string mName;  /// the name of the function

	std::vector<std::pair<DataType, std::string>> mDataInputs;
	std::vector<std::pair<DataType, std::string>> mDataOutputs;

	std::vector<std::string> mExecInputs;
	std::vector<std::string> mExecOutputs;
	
	
	std::unordered_map<std::string, std::unique_ptr<NodeInstance>>
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
} // namespace chig

#endif  // CHIG_GRAPH_FUNCTION_HPP
