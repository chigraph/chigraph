#!/usr/bin/env python
#=============================================================================
# Copyright 2016 by Shaheed Haque (srhaque@theiet.org)
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

"""SIP file generation rules engine."""

from __future__ import print_function

from abc import *
import argparse
import gettext
import inspect
import logging
import os
import re
import sys
import textwrap
import traceback
from copy import deepcopy
from clang.cindex import CursorKind

from clang.cindex import AccessSpecifier

class HelpFormatter(argparse.ArgumentDefaultsHelpFormatter, argparse.RawDescriptionHelpFormatter):
    pass


logger = logging.getLogger(__name__)
gettext.install(__name__)
_SEPARATOR = "\x00"

def _parents(container):
    parents = []
    parent = container.semantic_parent
    while parent and parent.kind != CursorKind.TRANSLATION_UNIT:
        parents.append(parent.spelling)
        parent = parent.semantic_parent
    if parents:
        parents = "::".join(reversed(parents))
    else:
        parents = os.path.basename(container.translation_unit.spelling)
    return parents


class Rule(object):
    def __init__(self, db, rule_number, fn, pattern_zip):
        self.db = db
        self.rule_number = rule_number
        self.fn = fn
        self.usage = 0
        try:
            groups = ["(?P<{}>{})".format(name, pattern) for pattern, name in pattern_zip]
            groups = _SEPARATOR.join(groups)
            self.matcher = re.compile(groups)
        except Exception as e:
            groups = ["{} '{}'".format(name, pattern) for pattern, name in pattern_zip]
            groups = ", ".join(groups)
            raise RuntimeError(_("Bad {}: {}: {}").format(self, groups, e))

    def match(self, candidate):
        return self.matcher.match(candidate)

    def trace_result(self, parents, item, original, modified):
        fqn = parents + "::" + original["name"] + "[" + str(item.extent.start.line) + "]"
        self._trace_result(fqn, original, modified)

    def _trace_result(self, fqn, original, modified):
        if not modified["name"]:
            logger.debug(_("Rule {} suppressed {}, {}").format(self, fqn, original))
        else:
            delta = False
            for k, v in original.iteritems():
                if v != modified[k]:
                    delta = True
                    break
            if delta:
                logger.debug(_("Rule {} modified {}, {}->{}").format(self, fqn, original, modified))
            else:
                logger.warn(_("Rule {} did not modify {}, {}").format(self, fqn, original))

    def __str__(self):
        return "[{},{}]".format(self.rule_number, self.fn.__name__)


class AbstractCompiledRuleDb(object):
    __metaclass__ = ABCMeta

    def __init__(self, db, parameter_names):
        self.db = db
        self.compiled_rules = []
        for i, raw_rule in enumerate(db()):
            if len(raw_rule) != len(parameter_names) + 1:
                raise RuntimeError(_("Bad raw rule {}: {}: {}").format(db.__name__, raw_rule, parameter_names))
            z = zip(raw_rule[:-1], parameter_names)
            self.compiled_rules.append(Rule(db, i, raw_rule[-1], z))
        self.candidate_formatter = _SEPARATOR.join(["{}"] * len(parameter_names))

    def _match(self, *args):
        candidate = self.candidate_formatter.format(*args)
        for rule in self.compiled_rules:
            matcher = rule.match(candidate)
            if matcher:
                #
                # Only use the first matching rule.
                #
                rule.usage += 1
                return matcher, rule
        return None, None

    @abstractmethod
    def apply(self, *args):
        raise NotImplemented(_("Missing subclass"))

    def dump_usage(self, fn):
        """ Dump the usage counts."""
        for rule in self.compiled_rules:
            fn(self.__class__.__name__, str(rule), rule.usage)


class ContainerRuleDb(AbstractCompiledRuleDb):
    """
    THE RULES FOR CONTAINERS.

    These are used to customise the behaviour of the SIP generator by allowing
    the declaration for any container (class, namespace, struct, union) to be
    customised, for example to add SIP compiler annotations.

    Each entry in the raw rule database must be a list with members as follows:

        0. A regular expression which matches the fully-qualified name of the
        "container" enclosing the container.

        1. A regular expression which matches the container name.

        2. A regular expression which matches any template parameters.

        3. A regular expression which matches the container declaration.

        4. A regular expression which matches any base specifiers.

        5. A function.

    In use, the database is walked in order from the first entry. If the regular
    expressions are matched, the function is called, and no further entries are
    walked. The function is called with the following contract:

        def container_xxx(container, sip, matcher):
            '''
            Return a modified declaration for the given container.

            :param container:   The clang.cindex.Cursor for the container.
            :param sip:         A dict with the following keys:

                                    name                The name of the container.
                                    template_parameters Any template parameters.
                                    decl                The declaration.
                                    base_specifiers     Any base specifiers.
                                    body                The body, less the outer
                                                        pair of braces.
                                    annotations         Any SIP annotations.

            :param matcher:         The re.Match object. This contains named
                                    groups corresponding to the key names above
                                    EXCEPT body and annotations.

            :return: An updated set of sip.xxx values. Setting sip.name to the
                     empty string will cause the container to be suppressed.
            '''

    :return: The compiled form of the rules.
    """
    def __init__(self, db):
        super(ContainerRuleDb, self).__init__(db, ["parents", "container", "template_parameters", "decl", "base_specifiers"])

    def apply(self, container, sip):
        """
        Walk over the rules database for functions, applying the first matching transformation.

        :param container:           The clang.cindex.Cursor for the container.
        :param sip:                 The SIP dict.
        """
        parents = _parents(container)
        matcher, rule = self._match(parents, sip["name"], sip["template_parameters"], sip["decl"], sip["base_specifiers"])
        if matcher:
            before = deepcopy(sip)
            rule.fn(container, sip, matcher)
            rule.trace_result(parents, container, before, sip)


class FunctionRuleDb(AbstractCompiledRuleDb):
    """
    THE RULES FOR FUNCTIONS.

    These are used to customise the behaviour of the SIP generator by allowing
    the declaration for any function to be customised, for example to add SIP
    compiler annotations.

    Each entry in the raw rule database must be a list with members as follows:

        0. A regular expression which matches the fully-qualified name of the
        "container" enclosing the function.

        1. A regular expression which matches the function name.

        2. A regular expression which matches any template parameters.

        3. A regular expression which matches the function result.

        4. A regular expression which matches the function parameters (e.g.
        "int a, void *b" for "int foo(int a, void *b)").

        5. A function.

    In use, the database is walked in order from the first entry. If the regular
    expressions are matched, the function is called, and no further entries are
    walked. The function is called with the following contract:

        def function_xxx(container, function, sip, matcher):
            '''
            Return a modified declaration for the given function.

            :param container:   The clang.cindex.Cursor for the container.
            :param function:    The clang.cindex.Cursor for the function.
            :param sip:         A dict with the following keys:

                                    name                The name of the function.
                                    template_parameters Any template parameters.
                                    fn_result           Result, if not a constructor.
                                    decl                The declaration.
                                    prefix              Leading keyworks ("static"). Separated by space,
                                                        ends with a space.
                                    suffix              Trailing keywords ("const"). Separated by space, starts with
                                                        space.
                                    annotations         Any SIP annotations.

            :param matcher:         The re.Match object. This contains named
                                    groups corresponding to the key names above
                                    EXCEPT annotations.

            :return: An updated set of sip.xxx values. Setting sip.name to the
                     empty string will cause the container to be suppressed.
            '''

    :return: The compiled form of the rules.
    """
    def __init__(self, db):
        super(FunctionRuleDb, self).__init__(db, ["container", "function", "template_parameters", "fn_result", "parameters"])

    def apply(self, container, function, sip):
        """
        Walk over the rules database for functions, applying the first matching transformation.

        :param container:           The clang.cindex.Cursor for the container.
        :param function:            The clang.cindex.Cursor for the function.
        :param sip:                 The SIP dict.
        """
        parents = _parents(function)
        matcher, rule = self._match(parents, sip["name"], ", ".join(sip["template_parameters"]), sip["fn_result"], ", ".join(sip["parameters"]))
        if matcher:
            before = deepcopy(sip)
            rule.fn(container, function, sip, matcher)
            rule.trace_result(parents, function, before, sip)


class ParameterRuleDb(AbstractCompiledRuleDb):
    """
    THE RULES FOR FUNCTION PARAMETERS.

    These are used to customise the behaviour of the SIP generator by allowing
    the declaration for any parameter in any function to be customised, for
    example to add SIP compiler annotations.

    Each entry in the raw rule database must be a list with members as follows:

        0. A regular expression which matches the fully-qualified name of the
        "container" enclosing the function enclosing the parameter.

        1. A regular expression which matches the function name enclosing the
        parameter.

        2. A regular expression which matches the parameter name.

        3. A regular expression which matches the parameter declaration (e.g.
        "int foo").

        4. A regular expression which matches the parameter initialiser (e.g.
        "Xyz:MYCONST + 42").

        5. A function.

    In use, the database is walked in order from the first entry. If the regular
    expressions are matched, the function is called, and no further entries are
    walked. The function is called with the following contract:

        def parameter_xxx(container, function, parameter, sip, init, matcher):
            '''
            Return a modified declaration and initialiser for the given parameter.

            :param container:   The clang.cindex.Cursor for the container.
            :param function:    The clang.cindex.Cursor for the function.
            :param parameter:   The clang.cindex.Cursor for the parameter.
            :param sip:         A dict with the following keys:

                                    name                The name of the function.
                                    decl                The declaration.
                                    init                Any initialiser.
                                    annotations         Any SIP annotations.

            :param matcher:         The re.Match object. This contains named
                                    groups corresponding to the key names above
                                    EXCEPT annotations.

            :return: An updated set of sip.xxx values.
        '''

    :return: The compiled form of the rules.
    """
    def __init__(self, db):
        super(ParameterRuleDb, self).__init__(db, ["container", "function", "parameter", "decl", "init"])

    def apply(self, container, function, parameter, sip):
        """
        Walk over the rules database for parameters, applying the first matching transformation.

        :param container:           The clang.cindex.Cursor for the container.
        :param function:            The clang.cindex.Cursor for the function.
        :param parameter:           The clang.cindex.Cursor for the parameter.
        :param sip:                 The SIP dict.
        """
        parents = _parents(function)
        matcher, rule = self._match(parents, function.spelling, sip["name"], sip["decl"], sip["init"])
        if matcher:
            before = deepcopy(sip)
            rule.fn(container, function, parameter, sip, matcher)
            rule.trace_result(parents, parameter, before, sip)


class VariableRuleDb(AbstractCompiledRuleDb):
    """
    THE RULES FOR VARIABLES.

    These are used to customise the behaviour of the SIP generator by allowing
    the declaration for any variable to be customised, for example to add SIP
    compiler annotations.

    Each entry in the raw rule database must be a list with members as follows:

        0. A regular expression which matches the fully-qualified name of the
        "container" enclosing the variable.

        1. A regular expression which matches the variable name.

        2. A regular expression which matches the variable declaration (e.g.
        "int foo").

        3. A function.

    In use, the database is walked in order from the first entry. If the regular
    expressions are matched, the function is called, and no further entries are
    walked. The function is called with the following contract:

        def variable_xxx(container, variable, sip, matcher):
            '''
            Return a modified declaration for the given variable.

            :param container:   The clang.cindex.Cursor for the container.
            :param variable:    The clang.cindex.Cursor for the variable.
            :param sip:         A dict with the following keys:

                                    name                The name of the variable.
                                    decl                The declaration.
                                    annotations         Any SIP annotations.

            :param matcher:         The re.Match object. This contains named
                                    groups corresponding to the key names above
                                    EXCEPT annotations.

            :return: An updated set of sip.xxx values. Setting sip.name to the
                     empty string will cause the container to be suppressed.
            '''

    :return: The compiled form of the rules.
    """
    def __init__(self, db):
        super(VariableRuleDb, self).__init__(db, ["container", "variable", "decl"])

    def apply(self, container, variable, sip):
        """
        Walk over the rules database for variables, applying the first matching transformation.

        :param container:           The clang.cindex.Cursor for the container.
        :param variable:            The clang.cindex.Cursor for the variable.
        :param sip:                 The SIP dict.
        """
        parents = _parents(variable)
        matcher, rule = self._match(parents, sip["name"], sip["decl"])
        if matcher:
            before = deepcopy(sip)
            rule.fn(container, variable, sip, matcher)
            rule.trace_result(parents, variable, before, sip)


class RuleSet(object):
    """
    To implement your own binding, create a subclass of RuleSet, also called
    RuleSet in your own Python module. Your subclass will expose the raw rules
    along with other ancilliary data exposed through the subclass methods.

    You then simply run the SIP generation and SIP compilation programs passing
    in the name of your rules file
    """
    __metaclass__ = ABCMeta

    @abstractmethod
    def container_rules(self):
        """
        Return a compiled list of rules for containers.

        :return: A ContainerRuleDb instance
        """
        raise NotImplemented(_("Missing subclass implementation"))

    @abstractmethod
    def function_rules(self):
        """
        Return a compiled list of rules for functions.

        :return: A FunctionRuleDb instance
        """
        raise NotImplemented(_("Missing subclass implementation"))

    @abstractmethod
    def parameter_rules(self):
        """
        Return a compiled list of rules for function parameters.

        :return: A ParameterRuleDb instance
        """
        raise NotImplemented(_("Missing subclass implementation"))

    @abstractmethod
    def variable_rules(self):
        """
        Return a compiled list of rules for variables.

        :return: A VariableRuleDb instance
        """
        raise NotImplemented(_("Missing subclass implementation"))

    def dump_unused(self):
        """Usage statistics, to identify unused rules."""
        def dumper(db_name, rule, usage):
            if usage:
                logger.info(_("Rule {}::{} used {} times".format(db_name, rule, usage)))
            else:
                logger.warn(_("Rule {}::{} unused".format(db_name, rule)))
        for db in [self.container_rules(), self.function_rules(), self.parameter_rules(),
                   self.variable_rules()]:
            db.dump_usage(dumper)


def container_discard(container, sip, matcher):
    sip["name"] = ""

def function_discard(container, function, sip, matcher):
    sip["name"] = ""

def parameter_transfer_to_parent(container, function, parameter, sip, matcher):
    if function.is_static_method():
        sip["annotations"].add("Transfer")
    else:
        sip["annotations"].add("TransferThis")

def param_rewrite_mode_t_as_int(container, function, parameter, sip, matcher):
    sip["decl"] = sip["decl"].replace("mode_t", "unsigned int")

def return_rewrite_mode_t_as_int(container, function, sip, matcher):
    sip["fn_result"] = "unsigned int"

def variable_discard(container, variable, sip, matcher):
    sip["name"] = ""

def parameter_strip_class_enum(container, function, parameter, sip, matcher):
    sip["decl"] = sip["decl"].replace("class ", "").replace("enum ", "")

def function_discard_impl(container, function, sip, matcher):
    if function.extent.start.column == 1:
        sip["name"] = ""

def rules(project_rules):
    """
    Constructor.

    :param project_rules:       The rules file for the project.
    """
    import imp
    imp.load_source("project_rules", project_rules)
    #
    # Statically prepare the rule logic. This takes the rules provided by the user and turns them into code.
    #
    return getattr(sys.modules["project_rules"], "RuleSet")()
