#!/usr/bin/env python
#=============================================================================
# Copyright 2016 Shaheed Haque <srhaque@theiet.org>
# Copyright 2016 Stephen Kelly <steveire@gmail.com>
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. The name of the author may not be used to endorse or promote products
#    derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
# NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#=============================================================================

from __future__ import print_function

import rules_engine

def container_rules():
    return [
        #
        # SIP does not seem to be able to handle these.
        #
        [".*", "(QMetaTypeId|QTypeInfo)<.*>", ".*", ".*", ".*", rules_engine.container_discard],
        #
        # SIP does not seem to be able to handle templated containers.
        #
        [".*", ".*<.*", ".*", ".*", ".*", rules_engine.container_discard],
    ]

def function_rules():
    return [
        #
        # Discard functions emitted by QOBJECT.
        #
        [".*", "metaObject|qt_metacast|tr|trUtf8|qt_metacall|qt_check_for_QOBJECT_macro|qt_check_for_QGADGET_macro", ".*", ".*", ".*", rules_engine.function_discard],
        #
        # SIP does not support operator=.
        #
        [".*", "operator=", ".*", ".*", ".*", rules_engine.function_discard],

        [".*", ".*", ".*", ".*", ".*::QPrivateSignal.*", rules_engine.function_discard],
        #
        # TODO: Temporarily remove any functions which require templates. SIP seems to support, e.g. QPairs,
        # but we have not made them work yet.
        #

        [".*", ".*", ".+", ".*", ".*", rules_engine.function_discard],
        [".*", ".*<.*>.*", ".*", ".*", ".*", rules_engine.function_discard],

        [".*", ".*", ".*", ".*", ".*std::function.*", rules_engine.function_discard],
        [".*", ".*", ".*", ".*", ".*std::numeric_limits.*", rules_engine.function_discard],
        [".*", ".*", ".*", ".*", ".*QPair.*", rules_engine.function_discard],
        [".*", ".*", ".*", ".*QPair.*", ".*", rules_engine.function_discard],

        [".*", ".*", ".*", ".*", ".*QDebug.*", rules_engine.function_discard],
        [".*", ".*", ".*", ".*QDebug.*", ".*", rules_engine.function_discard],

        [".*", ".*", ".*", ".*", ".*QExplicitlySharedDataPointer.*", rules_engine.function_discard],
        [".*", ".*", ".*", ".*QExplicitlySharedDataPointer.*", ".*", rules_engine.function_discard],

        [".*", ".*", ".*", ".*", ".*Private.*", rules_engine.function_discard],
        [".*", ".*", ".*", "mode_t", ".*", rules_engine.return_rewrite_mode_t_as_int],
        [".*", "d_func", ".*", ".*", ".*", rules_engine.function_discard],

        [".*", "operator\|", ".*", ".*", ".*", rules_engine.function_discard],
    ]

def parameter_rules():
    return [
        #
        # Annotate with Transfer or TransferThis when we see a parent object.
        #
        [".*", ".*", ".*", r"[KQ][A-Za-z_0-9]+\W*\*\W*parent", ".*", rules_engine.parameter_transfer_to_parent],
        [".*", ".*", ".*", "mode_t.*", ".*", rules_engine.param_rewrite_mode_t_as_int],
        [".*", ".*", ".*", ".*enum .*", ".*", rules_engine.parameter_strip_class_enum],
    ]

def variable_rules():
    return [
        #
        # Discard variable emitted by QOBJECT.
        #
        [".*", "staticMetaObject", ".*", rules_engine.variable_discard],
        #
        # Discard "private" variables.
        #
        [".*", "d_ptr", ".*", rules_engine.variable_discard],
        [".*", "d", ".*Private.*", rules_engine.variable_discard],
    ]

class RuleSet(rules_engine.RuleSet):
    """
    SIP file generator rules. This is a set of (short, non-public) functions
    and regular expression-based matching rules.
    """
    def __init__(self):
        self._container_db = rules_engine.ContainerRuleDb(container_rules)
        self._fn_db = rules_engine.FunctionRuleDb(function_rules)
        self._param_db = rules_engine.ParameterRuleDb(parameter_rules)
        self._var_db = rules_engine.VariableRuleDb(variable_rules)

    def container_rules(self):
        return self._container_db

    def function_rules(self):
        return self._fn_db

    def parameter_rules(self):
        return self._param_db

    def variable_rules(self):
        return self._var_db
