/// \file chi/FunctionValidator.hpp
/// Defines functions for validating GraphFunction objects

#pragma once

#ifndef CHI_FUNCTION_VALIDATOR_HPP
#define CHI_FUNCTION_VALIDATOR_HPP

#include "chi/Fwd.hpp"

namespace chi {

/// \name Function Validation
/// \brief Functions for making sure that generated functions are valid (also usable as a linter
/// tools)
/// \{

/// Validate that a function is compilable. If this succeeds, then it should be compilable
/// Calls validateFunctionConnectionsAreTwoWay and validateFunctionNodeInputs
/// \param func The function to check
/// \return The Result
Result validateFunction(const GraphFunction& func);

/// Make sure that connections connect back and that they have the same types
/// \param func The function to check
/// \return The Result
Result validateFunctionConnectionsAreTwoWay(const GraphFunction& func);

/// Make sure that nodes are called before their outputs are used
/// \param func The function to check
/// \return The Result
Result validateFunctionNodeInputs(const GraphFunction& func);

/// Make sure nodes have an output connection
/// \param func The function to check
/// \return The Result
Result validateFunctionExecOutputs(const GraphFunction& func);

/// Make sure the function entry type aligns with the function type
/// \param func The function to check
/// \return The Result
Result validateFunctionEntryType(const GraphFunction& func);

/// Make sure the function exit types align with the function type
/// \param func The function to check
/// \return The Result
Result validateFunctionExitTypes(const GraphFunction& func);

/// \}
}  // namespace chi

#endif  // CHI_FUNCTION_VALIDATOR_HPP
