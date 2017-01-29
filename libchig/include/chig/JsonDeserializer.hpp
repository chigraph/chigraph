#pragma once

#ifndef CHIG_JSON_DESERIALIZER_HPP
#define CHIG_JSON_DESERIALIZER_HPP

#include "chig/Fwd.hpp"
#include "chig/json.hpp"

#include <gsl/gsl>

namespace chig {

/// Load a JsonModule from json
/// \param createInside The Context to create the module in
/// \input The JSON to load
/// \toFill The JsonModule* to set, optional
Result deserializeJsonModule(Context& createInside, const nlohmann::json& input, gsl::cstring_span<> fullName, JsonModule** toFill = nullptr);

/// Create a forward declaration of a function in a module with an empty graph
/// \param createInside the JsonModule to create the forward declaration in
/// \param input The input JSON
/// \toFill The GraphFunction* to fill, optional
Result createGraphFunctionDeclarationFromJson(JsonModule& createInside, const nlohmann::json& input, GraphFunction** toFill = nullptr);

/// Load a GraphFunction--must already exist (use createGraphFunctionDeclarationFromJson)
/// \param createInside The GraphFunction to create the graph for
/// \input The JSON to load
Result deserializeGraphFunction(GraphFunction& createInside, const nlohmann::json& input);

} // namespace chig

#endif // CHIG_JSON_DESERIALIZER_HPP
