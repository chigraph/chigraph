/// \file chi/JsonDeserializer.hpp
/// Define deserialization functions

#pragma once

#ifndef CHI_JSON_DESERIALIZER_HPP
#define CHI_JSON_DESERIALIZER_HPP

#include "chi/Fwd.hpp"
#include "chi/json.hpp"

#include <boost/filesystem.hpp>
#include <boost/utility/string_view.hpp>

namespace chi {

/// \name Json Serialization/Deserialization
/// \{

/// Load a GraphModule from json
/// \param createInside The Context to create the module in
/// \param input The JSON to load
/// \param fullName The full name of the module being loaded
/// \retval toFill The GraphModule* to set, optional
Result jsonToGraphModule(Context& createInside, const nlohmann::json& input,
                         const boost::filesystem::path& fullName, GraphModule** toFill = nullptr);

/// Create a forward declaration of a function in a module with an empty graph
/// \param createInside the GraphModule to create the forward declaration in
/// \param input The input JSON
/// \retval toFill The GraphFunction* to fill, optional
Result createGraphFunctionDeclarationFromJson(GraphModule&          createInside,
                                              const nlohmann::json& input,
                                              GraphFunction**       toFill = nullptr);

/// Load a GraphFunction--must already exist (use createGraphFunctionDeclarationFromJson)
/// \param createInside The GraphFunction to create the graph for
/// \param input The JSON to load
Result jsonToGraphFunction(GraphFunction& createInside, const nlohmann::json& input);

/// Load a GraphStruct from json
/// \param mod The module to create it inside
/// \param input The JSON to load
/// \retval toFill The object to fill, optional
Result jsonToGraphStruct(GraphModule& mod, boost::string_view name, const nlohmann::json& input,
                         GraphStruct** toFill = nullptr);

/// Parse something that looks like: {"hello": "there"} into {"hello", "there"}
/// \param object The json object
/// \return {"key", "value"}
std::pair<std::string, std::string> parseObjectPair(const nlohmann::json& object);

/// \}

}  // namespace chi

#endif  // CHI_JSON_DESERIALIZER_HPP
