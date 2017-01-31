#pragma once

#ifndef CHIG_STRUCT_TYPE_HPP
#define CHIG_STRUCT_TYPE_HPP

#include <string>
#include <vector>

#include "chig/Fwd.hpp"
#include "chig/DataType.hpp"

namespace chig {

/// A class holding a compound type defined in a GraphModule
struct GraphStruct {
	
	/// GraphType constructor; don't use this use GraphModule::newStruct
	/// \param mod Module to add to
	/// \param name name of the type
	GraphStruct(GraphModule& mod, std::string name);

	/// Get the context
	/// \return The Context
	Context& context() const { return *mContext; }

	/// Get the module
	/// \return The GraphModule
	GraphModule& module() const { return *mModule; }
	
	/// Get the name of the type
	/// \return the name
	const std::string& name() const {return mName;}
	
	/// Get the types the struct contains
	/// \return The types
	const std::vector<std::pair<std::string, DataType>>& types() const { return mTypes; }
	
	/// Add a new type to the struct
	/// \param ty The type
	/// \param name The name of the type
	/// \param addBefore The type to add the new type before. Use types().size() to add to the end 
	void addType(DataType ty, std::string name, size_t addBefore);
	
	/// Change the type and name of a type
	/// \param id The ID to change
	/// \param newTy The new type
	/// \param newName The new name
	void modifyType(size_t id, DataType newTy, std::string newName);
	
	/// Remove a type from a struct
	/// \param id The ID to remove 
	void removeType(size_t id);
	
	/// Get the DataType of the struct
	DataType dataType();
	
private:
	GraphModule* mModule;
	Context*     mContext;

	std::vector<std::pair<std::string, DataType>> mTypes;
	
	std::string mName;
	
	DataType mDataType;
};
}

#endif  // CHIG_STRUCT_TYPE_HPP
