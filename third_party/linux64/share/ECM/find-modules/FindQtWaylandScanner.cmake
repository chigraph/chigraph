#.rst:
# FindQtWaylandScanner
# --------------------
#
# Try to find qtwaylandscanner.
#
# If the qtwaylandscanner executable is not in your PATH, you can provide
# an alternative name or full path location with the ``QtWaylandScanner_EXECUTABLE``
# variable.
#
# This will define the following variables:
#
# ``QtWaylandScanner_FOUND``
#     True if qtwaylandscanner is available
#
# ``QtWaylandScanner_EXECUTABLE``
#     The qtwaylandscanner executable.
#
# If ``QtWaylandScanner_FOUND`` is TRUE, it will also define the following imported
# target:
#
# ``Wayland::QtScanner``
#     The qtwaylandscanner executable.
#
# This module provides the following functions to generate C++ protocol
# implementations:
#
#   - ``ecm_add_qtwayland_client_protocol``
#   - ``ecm_add_qtwayland_server_protocol``
#
# ::
#
#   ecm_add_qtwayland_client_protocol(<source_files_var>
#                                     PROTOCOL <xmlfile>
#                                     BASENAME <basename>
#                                     [PREFIX <prefix>])
#
# Generate C++ wrapper to Wayland client protocol files from ``<xmlfile>``
# XML definition for the ``<basename>`` interface and append those files
# to ``<source_files_var>``.  Pass the ``<prefix>`` argument if the interface
# names don't start with ``qt_`` or ``wl_``.
#
# WaylandScanner is required and will be searched for.
#
# ::
#
#   ecm_add_qtwayland_server_protocol(<source_files_var>
#                                     PROTOCOL <xmlfile>
#                                     BASENAME <basename>
#                                     [PREFIX <prefix>])
#
# Generate C++ wrapper to Wayland server protocol files from ``<xmlfile>``
# XML definition for the ``<basename>`` interface and append those files
# to ``<source_files_var>``.  Pass the ``<prefix>`` argument if the interface
# names don't start with ``qt_`` or ``wl_``.
#
# WaylandScanner is required and will be searched for.
#
# Since 1.4.0.

#=============================================================================
# Copyright 2012-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

include(${CMAKE_CURRENT_LIST_DIR}/ECMFindModuleHelpersStub.cmake)

ecm_find_package_version_check(QtWaylandScanner)

# Find qtwaylandscanner
find_program(QtWaylandScanner_EXECUTABLE NAMES qtwaylandscanner HINTS /usr/lib/${CMAKE_LIBRARY_ARCHITECTURE}/qt5/bin/)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(QtWaylandScanner
    FOUND_VAR
        QtWaylandScanner_FOUND
    REQUIRED_VARS
        QtWaylandScanner_EXECUTABLE
)

mark_as_advanced(QtWaylandScanner_EXECUTABLE)

if(NOT TARGET Wayland::QtScanner AND QtWaylandScanner_FOUND)
    add_executable(Wayland::QtScanner IMPORTED)
    set_target_properties(Wayland::QtScanner PROPERTIES
        IMPORTED_LOCATION "${QtWaylandScanner_EXECUTABLE}"
    )
endif()

include(FeatureSummary)
set_package_properties(QtWaylandScanner PROPERTIES
    URL "http://qt.io"
    DESCRIPTION "Executable that converts XML protocol files to C++ code"
)


include(CMakeParseArguments)

function(ecm_add_qtwayland_client_protocol out_var)
    # Parse arguments
    set(oneValueArgs PROTOCOL BASENAME PREFIX)
    cmake_parse_arguments(ARGS "" "${oneValueArgs}" "" ${ARGN})

    if(ARGS_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "Unknown keywords given to ecm_add_qtwayland_client_protocol(): \"${ARGS_UNPARSED_ARGUMENTS}\"")
    endif()

    set(_prefix "${ARGS_PREFIX}")

    find_package(WaylandScanner REQUIRED QUIET)
    ecm_add_wayland_client_protocol(${out_var}
                                    PROTOCOL ${ARGS_PROTOCOL}
                                    BASENAME ${ARGS_BASENAME})

    get_filename_component(_infile ${ARGS_PROTOCOL} ABSOLUTE)
    set(_cheader "${CMAKE_CURRENT_BINARY_DIR}/wayland-${ARGS_BASENAME}-client-protocol.h")
    set(_header "${CMAKE_CURRENT_BINARY_DIR}/qwayland-${ARGS_BASENAME}.h")
    set(_code "${CMAKE_CURRENT_BINARY_DIR}/qwayland-${ARGS_BASENAME}.cpp")

    set_source_files_properties(${_header} ${_code} GENERATED)

    add_custom_command(OUTPUT "${_header}"
        COMMAND ${QtWaylandScanner_EXECUTABLE} client-header ${_infile} "" ${_prefix} > ${_header}
        DEPENDS ${_infile} ${_cheader} VERBATIM)

    add_custom_command(OUTPUT "${_code}"
        COMMAND ${QtWaylandScanner_EXECUTABLE} client-code ${_infile} "" ${_prefix} > ${_code}
        DEPENDS ${_infile} ${_header} VERBATIM)

    list(APPEND ${out_var} "${_code}")
    set(${out_var} ${${out_var}} PARENT_SCOPE)
endfunction()


function(ecm_add_qtwayland_server_protocol out_var)
    # Parse arguments
    set(oneValueArgs PROTOCOL BASENAME PREFIX)
    cmake_parse_arguments(ARGS "" "${oneValueArgs}" "" ${ARGN})

    if(ARGS_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "Unknown keywords given to ecm_add_qtwayland_server_protocol(): \"${ARGS_UNPARSED_ARGUMENTS}\"")
    endif()

    set(_prefix "${ARGS_PREFIX}")

    find_package(WaylandScanner REQUIRED QUIET)
    ecm_add_wayland_server_protocol(${out_var}
                                    PROTOCOL ${ARGS_PROTOCOL}
                                    BASENAME ${ARGS_BASENAME})

    get_filename_component(_infile ${ARGS_PROTOCOL} ABSOLUTE)
    set(_header "${CMAKE_CURRENT_BINARY_DIR}/qwayland-server-${ARGS_BASENAME}.h")
    set(_code "${CMAKE_CURRENT_BINARY_DIR}/qwayland-server-${ARGS_BASENAME}.cpp")

    set_source_files_properties(${_header} ${_code} GENERATED)

    add_custom_command(OUTPUT "${_header}"
        COMMAND ${QtWaylandScanner_EXECUTABLE} server-header ${_infile} "" ${_prefix} > ${_header}
        DEPENDS ${_infile} VERBATIM)

    add_custom_command(OUTPUT "${_code}"
        COMMAND ${QtWaylandScanner_EXECUTABLE} server-code ${_infile} "" ${_prefix} > ${_code}
        DEPENDS ${_infile} ${_header} VERBATIM)

    list(APPEND ${out_var} "${_code}")
    set(${out_var} ${${out_var}} PARENT_SCOPE)
endfunction()
