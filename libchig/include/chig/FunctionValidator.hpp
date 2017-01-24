/// \file chig/FunctionValidator.hpp
/// Defines functions for validating GraphFunction objects


#pragma once

#ifndef CHIG_FUNCTION_VALIDATOR_HPP
#define CHIG_FUNCTION_VALIDATOR_HPP

#include "chig/Fwd.hpp"
#include "chig/Result.hpp"

namespace chig {

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

/// The make sure that nodes are called before their outputs are used
/// \param func The function to check
/// \return The Result
Result validateFunctionNodeInputs(const GraphFunction& func);

/// \}
}

#endif  // CHIG_FUNCTION_VALIDATOR_HPP
