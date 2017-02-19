#pragma once

#ifndef CHIG_JSON_SERIALIZER_HPP
#define CHIG_JSON_SERIALIZER_HPP

#include "chig/Fwd.hpp"
#include "chig/json.hpp"

namespace chig {

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
}

#endif  // CHIG_JSON_SERIALIZER_HPP
