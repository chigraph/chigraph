#.rst:
# ECMPoQmTools
# ------------
#
# This module provides the ``ecm_process_po_files_as_qm`` and
# ``ecm_install_po_files_as_qm`` functions for generating QTranslator (.qm)
# catalogs from Gettext (.po) catalogs, and the ``ecm_create_qm_loader``
# function for generating the necessary code to load them in a Qt application
# or library.
#
# ::
#
#   ecm_process_po_files_as_qm(<lang> [ALL]
#                              [INSTALL_DESTINATION <install_destination>]
#                              PO_FILES <pofile> [<pofile> [...]])
#
# Compile .po files into .qm files for the given language.
#
# If INSTALL_DESTINATION is given, the .qm files are installed in
# ``<install_destination>/<lang>/LC_MESSAGES``. Typically,
# ``<install_destination>`` is set to ``share/locale``.
#
# ``ecm_process_po_files_as_qm`` creates a "translations" target. This target
# builds all .po files into .qm files.  If ALL is specified, these rules are
# added to the "all" target (and so the .qm files will be built by default).
#
# ::
#
#   ecm_create_qm_loader(<source_files_var> <catalog_name>)
#
# Generates C++ code which ensures translations are automatically loaded at
# startup. The generated files are appended to ``<source_files_var>``.
#
# It assumes that the .qm file for the language code ``<lang>`` is installed as
# ``<sharedir>/locale/<lang>/LC_MESSAGES/<catalog_name>.qm``, where
# ``<sharedir>`` is one of the directories given by the ``GenericDataLocation``
# of ``QStandardPaths``.
#
# Typical usage is like:
#
# .. code-block:: cmake
#
#   set(mylib_SRCS foo.cpp bar.cpp)
#   ecm_create_qm_loader(mylib_SRCS mylib)
#   add_library(mylib ${mylib_SRCS})
#
# ::
#
#   ecm_install_po_files_as_qm(<podir>)
#
# Searches for .po files and installs them to the standard location.
#
# This is a convenience function which relies on all .po files being kept in
# ``<podir>/<lang>/``, where ``<lang>`` is the language the .po files are
# written in.
#
# For example, given the following directory structure::
#
#  po/
#    fr/
#      mylib.po
#
# ``ecm_install_po_files_as_qm(po)`` compiles ``mylib.po`` into ``mylib.mo`` and
# installs it in ``<install_destination>/fr/LC_MESSAGES``.
# ``<install_destination>`` defaults to ``${LOCALE_INSTALL_DIR}`` if defined,
# otherwise it uses ``${CMAKE_INSTALL_LOCALEDIR}`` if that is defined, otherwise
# it uses ``share/locale``.
#
# Since pre-1.0.0.

#=============================================================================
# Copyright 2007-2009 Kitware, Inc.
# Copyright 2007      Alexander Neundorf <neundorf@kde.org>
# Copyright 2014      Aurélien Gâteau <agateau@kde.org>
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

# Copied from FindGettext.cmake
function(_ecm_qm_get_unique_target_name _name _unique_name)
   set(propertyName "_ECM_QM_UNIQUE_COUNTER_${_name}")
   get_property(currentCounter GLOBAL PROPERTY "${propertyName}")
   if(NOT currentCounter)
      set(currentCounter 1)
   endif()
   set(${_unique_name} "${_name}_${currentCounter}" PARENT_SCOPE)
   math(EXPR currentCounter "${currentCounter} + 1")
   set_property(GLOBAL PROPERTY ${propertyName} ${currentCounter} )
endfunction()


function(ecm_create_qm_loader out_var catalog_name)
    set(loader_base ${CMAKE_CURRENT_BINARY_DIR}/ECMQmLoader-${catalog_name})

    set(QM_LOADER_CATALOG_NAME "${catalog_name}")

    configure_file(
        ${ECM_MODULE_DIR}/ECMQmLoader.cpp.in
        "${loader_base}.cpp"
        @ONLY
    )
    set(${out_var} "${${out_var}}" "${loader_base}.cpp" PARENT_SCOPE)
endfunction()


function(ecm_process_po_files_as_qm lang)
    # Parse arguments
    set(options ALL)
    set(oneValueArgs INSTALL_DESTINATION)
    set(multiValueArgs PO_FILES)
    cmake_parse_arguments(ARGS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(ARGS_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "Unknown keywords given to ecm_process_po_files_as_qm(): \"${ARGS_UNPARSED_ARGUMENTS}\"")
    endif()

    if(NOT ARGS_PO_FILES)
        message(FATAL_ERROR "ecm_process_po_files_as_qm() must be called with PO_FILES argument")
    endif()

    # Find lrelease and lconvert
    find_package(Qt5LinguistTools CONFIG REQUIRED)

    if(TARGET Qt5::lconvert)
        set(lconvert_executable Qt5::lconvert)
    else()
        # Qt < 5.3.1 does not define Qt5::lconvert
        get_target_property(lrelease_location Qt5::lrelease LOCATION)
        get_filename_component(lrelease_path ${lrelease_location} PATH)
        find_program(lconvert_executable
            NAMES lconvert-qt5 lconvert
            PATHS ${lrelease_path}
            NO_DEFAULT_PATH
            )
    endif()

    # Create commands to turn po files into qm files
    set(qm_files)
    foreach (po_file ${ARGS_PO_FILES})
        get_filename_component(po_file ${po_file} ABSOLUTE)
        get_filename_component(filename_base ${po_file} NAME_WE)

        # Include ${lang} in build dir because we might be called multiple times
        # with the same ${filename_base}
        set(build_dir ${CMAKE_CURRENT_BINARY_DIR}/${lang})
        set(ts_file ${build_dir}/${filename_base}.ts)
        set(qm_file ${build_dir}/${filename_base}.qm)

        file(MAKE_DIRECTORY ${build_dir})

        # lconvert from .po to .ts, then lrelease from .ts to .qm.
        add_custom_command(OUTPUT ${qm_file}
            COMMAND ${lconvert_executable}
                ARGS -i ${po_file} -o ${ts_file} -target-language ${lang}
            COMMAND Qt5::lrelease
                ARGS -removeidentical -nounfinished -silent ${ts_file} -qm ${qm_file}
            DEPENDS ${po_file}
            )
        if (ARGS_INSTALL_DESTINATION)
            install(
                FILES ${qm_file}
                DESTINATION ${ARGS_INSTALL_DESTINATION}/${lang}/LC_MESSAGES
            )
        endif()
        list(APPEND qm_files ${qm_file})
    endforeach()

    # Hook qm files to targets
    if(NOT TARGET translations)
        add_custom_target(translations)
    endif()

    _ecm_qm_get_unique_target_name(translations target_name)

    if (ARGS_ALL)
        add_custom_target(${target_name} ALL DEPENDS ${qm_files})
    else()
        add_custom_target(${target_name} DEPENDS ${qm_files})
    endif()

    add_dependencies(translations ${target_name})
endfunction()


function(ecm_install_po_files_as_qm podir)
    if (LOCALE_INSTALL_DIR)
        set(install_destination "${LOCALE_INSTALL_DIR}")
    elseif (CMAKE_INSTALL_LOCALEDIR)
        set(install_destination "${CMAKE_INSTALL_LOCALEDIR}")
    else()
        set(install_destination share/locale)
    endif()

    file(GLOB po_files "${podir}/*/*.po")
    foreach(po_file ${po_files})
        get_filename_component(po_dir ${po_file} DIRECTORY)
        get_filename_component(lang ${po_dir} NAME)
        ecm_process_po_files_as_qm(
            ${lang} ALL
            PO_FILES ${po_file}
            INSTALL_DESTINATION ${install_destination}
        )
    endforeach()
endfunction()
