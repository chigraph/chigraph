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
/// \param[in] createInside The Context to create the module in
/// \param[in] input The JSON to load
/// \param[in] fullName The full name of the module being loaded
/// \param[out] toFill The GraphModule* to set, optional
/// \return The Result
Result jsonToGraphModule(Context& createInside, const nlohmann::json& input,
                         const boost::filesystem::path& fullName, GraphModule** toFill = nullptr);

/// Create a forward declaration of a function in a module with an empty graph
/// \param[in] createInside the GraphModule to create the forward declaration in
/// \param[in] input The input JSON
/// \param[out] toFill The GraphFunction* to fill, optional
/// \return The Result
Result createGraphFunctionDeclarationFromJson(GraphModule&          createInside,
                                              const nlohmann::json& input,
                                              GraphFunction**       toFill = nullptr);

/// Load a GraphFunction--must already exist (use createGraphFunctionDeclarationFromJson)
/// \param createInside The GraphFunction to create the graph for
/// \param input The JSON to load
/// \return The Result
Result jsonToGraphFunction(GraphFunction& createInside, const nlohmann::json& input);

/// Load a GraphStruct from json
/// \param[in] mod The module to create it inside
/// \param[in] name The name of the `GraphStruct` to create
/// \param[in] input The JSON to load
/// \param[out] toFill The object to fill, optional
/// \return The Result
Result jsonToGraphStruct(GraphModule& mod, boost::string_view name, const nlohmann::json& input,
                         GraphStruct** toFill = nullptr);

/// Parse something that looks like: {"hello": "there"} into {"hello", "there"}
/// \param object The json object
/// \return {"key", "value"}
std::pair<std::string, std::string> parseObjectPair(const nlohmann::json& object);

/// \}

}  // namespace chi

#endif  // CHI_JSON_DESERIALIZER_HPP
