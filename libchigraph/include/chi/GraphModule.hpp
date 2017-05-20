/// \file chi/GraphModule.hpp
/// Defines the GraphModule class

#ifndef CHI_GRAPH_MODULE_HPP
#define CHI_GRAPH_MODULE_HPP

#pragma once

#include <chi/ChiModule.hpp>
#include <chi/Fwd.hpp>

#include <boost/bimap.hpp>

namespace chi {
/// Module that holds graph functions
struct GraphModule : public ChiModule {
	/// Construct a GraphModule
	/// \param cont The context
	/// \param fullName The full name of the module
	/// \param dependencies The dependencies
	GraphModule(Context& cont, boost::filesystem::path fullName,
	            const std::vector<boost::filesystem::path>& dependencies = {});

	// No copy or move -- pointer only
	GraphModule(const GraphModule&) = delete;
	GraphModule(GraphModule&&)      = delete;
	GraphModule& operator=(const GraphModule&) = delete;
	GraphModule& operator=(GraphModule&&) = delete;

	// ChiModule interface
	///////////////////////

	Result nodeTypeFromName(boost::string_view name, const nlohmann::json& jsonData,
	                        std::unique_ptr<NodeType>* toFill) override;

	DataType typeFromName(boost::string_view name) override;
	std::vector<std::string> nodeTypeNames() const override;

	std::vector<std::string> typeNames() const override;

	Result addForwardDeclarations(llvm::Module& module) const override;

	Result generateModule(llvm::Module& module) override;

	/////////////////////

	/// Create the associations from line number and function in debug info
	/// \return A bimap of function to line number
	boost::bimap<unsigned, NodeInstance*> createLineNumberAssoc() const;

	/// Serialize to disk in the context
	/// \return The Result
	Result saveToDisk() const;

	/// Get the path to the source file
	/// It's not garunteed to exist, because it could have not been saved
	/// \return The path
	boost::filesystem::path sourceFilePath() const;

	/// \name Function Creation and Manipulation
	/// \{

	/// Create a new function if it does't already exist
	/// \param name The name of the new function
	/// \param dataIns The data inputs to the function
	/// \param dataOuts The data outputs to the function
	/// \param execIns The exec inputs to the function
	/// \param execOuts The exec outputs to the function
	/// \param inserted Pointer of a bool to fill; sets to true if it was inserted and false if it
	/// already existed
	/// \return The function
	GraphFunction* getOrCreateFunction(std::string name, std::vector<NamedDataType> dataIns,
	                                   std::vector<NamedDataType> dataOuts,
	                                   std::vector<std::string>   execIns,
	                                   std::vector<std::string> execOuts, bool* inserted = nullptr);

	/// Remove a function from the module
	/// \param name The name of the function to remove
	/// \param deleteReferences should all the references in the context be deleted?
	/// \return True if there was a function matching name that was removed
	bool removeFunction(boost::string_view name, bool deleteReferences = true);

	/// Remove a function from the module
	/// \param func The function to remove
	/// \param deleteReferences should all the references in the context be deleted?
	void removeFunction(GraphFunction& func, bool deleteReferences = true);

	/// Get a function from the name
	/// \param name The name to get
	/// \return The GraphFunction or nullptr if it doesn't exist
	GraphFunction* functionFromName(boost::string_view name) const;

	/// Get functions
	/// \return The functions
	const std::vector<std::unique_ptr<GraphFunction>>& functions() const { return mFunctions; }

	///\}

	/// \name Struct Creation and Manipulation
	/// \{

	const std::vector<std::unique_ptr<GraphStruct>>& structs() const { return mStructs; }

	/// Get a struct by name
	/// \param name The name of the struct
	/// \return The struct or nullptr if not found
	GraphStruct* structFromName(boost::string_view name) const;

	/// Create a new struct in the module
	/// \param name The name of the struct
	/// \param inserted True if the struct was new, optional
	/// \return The new struct
	GraphStruct* getOrCreateStruct(std::string name, bool* inserted = nullptr);

	/// Remove a struct from the module by name
	/// \param name The name of the struct to remove
	/// \return True if a struct was actually removed, false if no struct by that name existed
	bool removeStruct(boost::string_view name);

	/// Remove a struct from the module by pointer
	/// \param tyToDel Struct to delete, must be in this module
	/// \expects `tyToDel->module() == this`
	void removeStruct(GraphStruct* tyToDel);

	/// \}

	/// \{
	/// \name C Support

	/// Set if C support is enabled
	/// \param newValue true to enable C support, false to disable it
	void setCEnabled(bool newValue) { mCEnabled = true; }

	/// Gets if C support is enabled
	/// \return true if C support is enabled
	bool cEnabled() const { return mCEnabled; }

	/// From C code, create a NodeType
	/// \param code The code
	/// \param functionName the function to call
	/// \param clangArgs Extra arguments to pass to clang
	/// \param toFill The unique_ptr to fill
	/// \pre toFill must not be nullptr (the unique_ptr it points to can be though)
	/// \return The Result
	Result createNodeTypeFromCCode(boost::string_view code, boost::string_view functionName,
	                               std::vector<std::string>   clangArgs,
	                               std::unique_ptr<NodeType>* toFill);

	/// Get the path to the .c directory. It is not garunteed to exist, even if cEnabled() is true
	boost::filesystem::path pathToCSources() const {
		return sourceFilePath().parent_path() / (shortName() + ".c");
	}

	/// \}

private:
	std::vector<std::unique_ptr<GraphFunction>> mFunctions;
	std::vector<std::unique_ptr<GraphStruct>>   mStructs;

	bool mCEnabled = false;
};
}  // namespace chi

#endif  // CHI_GRAPH_MODULE_HPP
