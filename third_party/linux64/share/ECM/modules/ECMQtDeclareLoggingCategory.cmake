#.rst:
# ECMQtDeclareLoggingCategory
# ---------------------------
#
# Generate declarations for logging categories in Qt5.
#
# ::
#
#   ecm_qt_declare_logging_category(<sources_var>
#                                   HEADER <filename>
#                                   IDENTIFIER <identifier>
#                                   CATEGORY_NAME <category_name>
#                                   [DEFAULT_SEVERITY
#                                        <Debug|Info|Warning|
#                                         Critical|Fatal>])
#
# A header file, ``<filename>``, will be generated along with a corresponding
# source file, which will be added to ``<sources_var>``. These will provide a
# QLoggingCategory category that can be referred to from C++ code using
# ``<identifier>``, and from the logging configuration using
# ``<category_name>``.
#
# If ``<filename>`` is not absolute, it will be taken relative to the current
# binary directory.
#
# If the code is compiled against Qt 5.4 or later, by default it will only log
# output that is at least the severity specified by ``DEFAULT_SEVERITY``, or
# "Info" level if ``DEFAULT_SEVERITY`` is not given. Note that, due to a
# bug in Qt 5.5, "Info" may be treated as more severe than "Fatal".
#
# ``<identifier>`` may include namespaces (eg: ``foo::bar::IDENT``).
#
# Since 5.14.0.

#=============================================================================
# Copyright 2015 Alex Merry <alex.merry@kde.org>
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

include(CMakeParseArguments)

set(_ECM_QT_DECLARE_LOGGING_CATEGORY_TEMPLATE_CPP "${CMAKE_CURRENT_LIST_DIR}/ECMQtDeclareLoggingCategory.cpp.in")
set(_ECM_QT_DECLARE_LOGGING_CATEGORY_TEMPLATE_H   "${CMAKE_CURRENT_LIST_DIR}/ECMQtDeclareLoggingCategory.h.in")

function(ecm_qt_declare_logging_category sources_var)
    set(options)
    set(oneValueArgs HEADER IDENTIFIER CATEGORY_NAME DEFAULT_SEVERITY)
    set(multiValueArgs)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(ARG_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "Unexpected arguments to ecm_qt_declare_logging_category: ${ARG_UNPARSED_ARGUMENTS}")
    endif()
    if(NOT ARG_HEADER)
        message(FATAL_ERROR "Missing HEADER argument for ecm_qt_declare_logging_category")
    endif()
    if(NOT ARG_IDENTIFIER)
        message(FATAL_ERROR "Missing IDENTIFIER argument for ecm_qt_declare_logging_category")
    endif()
    if(NOT ARG_CATEGORY_NAME)
        message(FATAL_ERROR "Missing CATEGORY_NAME argument for ecm_qt_declare_logging_category")
    endif()
    if(NOT ARG_DEFAULT_SEVERITY)
        set(ARG_DEFAULT_SEVERITY Info)
    else()
        set(acceptible_severities Debug Info Warning Critical Fatal)
        list(FIND acceptible_severities "${ARG_DEFAULT_SEVERITY}" pos)
        if (pos EQUAL -1)
            message(FATAL_ERROR "Unknown DEFAULT_SEVERITY ${pos}")
        endif()
    endif()

    if (NOT IS_ABSOLUTE "${ARG_HEADER}")
        set(ARG_HEADER "${CMAKE_CURRENT_BINARY_DIR}/${ARG_HEADER}")
    endif()

    string(REPLACE "::" ";" namespaces "${ARG_IDENTIFIER}")
    list(LENGTH namespaces len)
    math(EXPR last_pos "${len} - 1")
    list(GET namespaces ${last_pos} IDENTIFIER)
    list(REMOVE_AT namespaces ${last_pos})

    set(OPEN_NAMESPACES)
    set(CLOSE_NAMESPACES)
    foreach(ns ${namespaces})
        set(OPEN_NAMESPACES "${OPEN_NAMESPACES} namespace ${ns} {")
        set(CLOSE_NAMESPACES "} ${CLOSE_NAMESPACES}")
    endforeach()

    string(FIND "${ARG_HEADER}" "." pos REVERSE)
    if (pos EQUAL -1)
        set(cpp_filename "${ARG_HEADER}.cpp")
    else()
        string(SUBSTRING "${ARG_HEADER}" 0 ${pos} cpp_filename)
        set(cpp_filename "${cpp_filename}.cpp")
    endif()

    get_filename_component(HEADER_NAME "${ARG_HEADER}" NAME)

    string(REPLACE "::" "_" GUARD_NAME "${ARG_IDENTIFIER}_H")
    string(TOUPPER "${GUARD_NAME}" GUARD_NAME)

    if (NOT _ECM_QT_DECLARE_LOGGING_CATEGORY_TEMPLATE_CPP)
       message(FATAL_ERROR "You must include(ECMQtDeclareLoggingCategory) before using ecm_qt_declare_logging_category")
    endif()

    configure_file("${_ECM_QT_DECLARE_LOGGING_CATEGORY_TEMPLATE_CPP}" "${cpp_filename}")
    configure_file("${_ECM_QT_DECLARE_LOGGING_CATEGORY_TEMPLATE_H}" "${ARG_HEADER}")

    set(sources "${${sources_var}}")
    list(APPEND sources "${cpp_filename}")
    set(${sources_var} "${sources}" PARENT_SCOPE)
endfunction()
