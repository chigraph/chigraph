/// \file chi/JsonSerializer.hpp
/// Define json serialization functions

#pragma once

#ifndef CHI_JSON_SERIALIZER_HPP
#define CHI_JSON_SERIALIZER_HPP

#include "chi/Fwd.hpp"
#include "chi/json.hpp"

namespace chi {

/// \name Json Serialization/Deserialization
/// \{

/// Serialize a GraphFunction to json
/// \param func The function to serialize
/// \return The serialized function
nlohmann::json graphFunctionToJson(const GraphFunction& func);

/// Serialize a JsonModule to json
/// \param mod The module to serialize
/// \return The serialized module
nlohmann::json graphModuleToJson(const GraphModule& mod);

/// Serialize a GraphStruct to json
/// \param struc The struct to serialize
nlohmann::json graphStructToJson(const GraphStruct& struc);

/// \}
}  // namespace chi

#endif  // CHI_JSON_SERIALIZER_HPP
