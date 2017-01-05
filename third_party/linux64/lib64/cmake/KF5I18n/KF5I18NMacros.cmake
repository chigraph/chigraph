
# Copyright (c) 2006, Alexander Neundorf, <neundorf@kde.org>
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. Neither the name of the University nor the names of its contributors
#    may be used to endorse or promote products derived from this software
#    without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.

find_package(Gettext REQUIRED)
find_package(PythonInterp REQUIRED)

set(_ki18n_pmap_compile_script ${CMAKE_CURRENT_LIST_DIR}/ts-pmap-compile.py)
set(_ki18n_uic_script ${CMAKE_CURRENT_LIST_DIR}/kf5i18nuic.cmake)

#create the implementation files from the ui files and add them to the list of sources
#usage: KI18N_WRAP_UI(foo_SRCS ${ui_files})
macro (KI18N_WRAP_UI _sources )
   foreach (_current_FILE ${ARGN})

      get_filename_component(_tmp_FILE ${_current_FILE} ABSOLUTE)
      get_filename_component(_basename ${_tmp_FILE} NAME_WE)
      set(_header ${CMAKE_CURRENT_BINARY_DIR}/ui_${_basename}.h)

      get_target_property(QT_UIC_EXECUTABLE Qt5::uic LOCATION)
      # we need to run uic and replace some things in the generated file
      # this is done by executing the cmake script kf5i18nuic.cmake
      add_custom_command(OUTPUT ${_header}
         COMMAND ${CMAKE_COMMAND}
         ARGS
         -DKDE_UIC_EXECUTABLE:FILEPATH=${QT_UIC_EXECUTABLE}
         -DKDE_UIC_FILE:FILEPATH=${_tmp_FILE}
         -DKDE_UIC_H_FILE:FILEPATH=${_header}
         -DKDE_UIC_BASENAME:STRING=${_basename}
         -P ${_ki18n_uic_script}
         MAIN_DEPENDENCY ${_tmp_FILE}
      )
      list(APPEND ${_sources} ${_header})
   endforeach (_current_FILE)
endmacro (KI18N_WRAP_UI)

#install the scripts for a given language in the target folder
#usage: KI18N_INSTALL_TS_FILES("ja" ${scripts_dir})
function(KI18N_INSTALL_TS_FILES lang scripts_dir)
   file(GLOB_RECURSE ts_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${scripts_dir}/*)
   set(pmapc_files)
   foreach(ts_file ${ts_files})
      string(REGEX MATCH "\\.svn/" in_svn ${ts_file})
      if(NOT in_svn)
         # If ts_file is "path/to/foo/bar.js"
         # We want subpath to contain "foo"
         get_filename_component(subpath ${ts_file} DIRECTORY)
         get_filename_component(subpath ${subpath} NAME)
         install(FILES ${ts_file}
                 DESTINATION ${LOCALE_INSTALL_DIR}/${lang}/LC_SCRIPTS/${subpath})
         # If current file is a pmap, also install the compiled version.
         get_filename_component(ts_ext ${ts_file} EXT)
         if(ts_ext STREQUAL ".pmap")
            set(pmap_file ${ts_file})
            get_filename_component(pmap_basename ${ts_file} NAME)
            set(pmapc_basename "${pmap_basename}c")
            set(pmapc_file "${lang}-${subpath}-${pmapc_basename}")
            add_custom_command(OUTPUT ${pmapc_file}
               COMMAND ${PYTHON_EXECUTABLE}
               ARGS
               -B
               ${_ki18n_pmap_compile_script}
               ${CMAKE_CURRENT_SOURCE_DIR}/${pmap_file}
               ${pmapc_file}
               DEPENDS ${pmap_file})
            install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${pmapc_file}
                    DESTINATION ${LOCALE_INSTALL_DIR}/${lang}/LC_SCRIPTS/${subpath}
                    RENAME ${pmapc_basename})
            list(APPEND pmapc_files ${pmapc_file})
         endif()
      endif()
   endforeach()
   if(pmapc_files)
      if(NOT TARGET pmapfiles)
         add_custom_target(pmapfiles)
      endif()
      set(pmapc_target "pmapfiles-${lang}")
      add_custom_target(${pmapc_target} ALL DEPENDS ${pmapc_files})
      add_dependencies(pmapfiles ${pmapc_target})
   endif()
endfunction()

# KI18N_INSTALL(podir)
# Search for .po files and scripting modules and install them to the standard
# location.
#
# This is a convenience function which relies on the following directory
# structure:
#
#  <podir>/
#    <lang>/
#      scripts/
#        <domain>/
#          *.js
#      *.po
#
# .po files are passed to the GETTEXT_PROCESS_PO_FILES function from the
# CMake Gettext module.
#
# .js files are installed using KI18N_INSTALL_TS_FILES.
#
# For example, given the following directory structure:
#
#  po/
#    fr/
#      scripts/
#        kfoo/
#          kfoo.js
#      kfoo.po
#
# KI18N_INSTALL(po) does the following:
# - Compiles kfoo.po into kfoo.mo and installs it in
#   ${LOCALE_INSTALL_DIR}/fr/LC_MESSAGES or share/locale/fr/LC_MESSAGES if
#   ${LOCALE_INSTALL_DIR} is not set.
# - Installs kfoo.js in ${LOCALE_INSTALL_DIR}/fr/LC_SCRIPTS/kfoo
function(KI18N_INSTALL podir)
    file(GLOB lang_dirs "${podir}/*")
    if (NOT LOCALE_INSTALL_DIR)
        set(LOCALE_INSTALL_DIR share/locale)
    endif()
    foreach(lang_dir ${lang_dirs})
        get_filename_component(lang ${lang_dir} NAME)

        file(GLOB po_files "${lang_dir}/*.po")
        _ki18n_gettext_process_po_files(${lang} ALL
            INSTALL_DESTINATION ${LOCALE_INSTALL_DIR}
            PO_FILES ${po_files}
        )
        ki18n_install_ts_files(${lang} ${lang_dir}/scripts)
    endforeach()
endfunction()


###############################################################################
# The following code has been copied from CMake FindGettext.cmake and adjusted
# to support processing multiple .po files with the same name in different
# directories.
#
# CMake bug report: http://public.kitware.com/Bug/view.php?id=14904
#
# This code comes with the following license notice:
#=============================================================================
# Copyright 2007-2009 Kitware, Inc.
# Copyright 2007      Alexander Neundorf <neundorf@kde.org>
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
function(_KI18N_GETTEXT_PROCESS_PO_FILES _lang)
   set(_options ALL)
   set(_oneValueArgs INSTALL_DESTINATION)
   set(_multiValueArgs PO_FILES)
   set(_gmoFiles)

   CMAKE_PARSE_ARGUMENTS(_parsedArguments "${_options}" "${_oneValueArgs}" "${_multiValueArgs}" ${ARGN})

   foreach(_current_PO_FILE ${_parsedArguments_PO_FILES})
      get_filename_component(_name ${_current_PO_FILE} NAME)
      string(REGEX REPLACE "^(.+)(\\.[^.]+)$" "\\1" _basename ${_name})
      set(_gmoFile ${CMAKE_CURRENT_BINARY_DIR}/${_lang}-${_basename}.gmo)
      add_custom_command(OUTPUT ${_gmoFile}
            COMMAND ${GETTEXT_MSGFMT_EXECUTABLE} -o ${_gmoFile} ${_current_PO_FILE}
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
            DEPENDS ${_current_PO_FILE}
         )

      if(_parsedArguments_INSTALL_DESTINATION)
         install(FILES ${_gmoFile} DESTINATION ${_parsedArguments_INSTALL_DESTINATION}/${_lang}/LC_MESSAGES/ RENAME ${_basename}.mo)
      endif()
      list(APPEND _gmoFiles ${_gmoFile})
   endforeach()


  if(NOT TARGET pofiles)
     add_custom_target(pofiles)
  endif()

  _KI18N_GETTEXT_GET_UNIQUE_TARGET_NAME( pofiles uniqueTargetName)

   if(_parsedArguments_ALL)
      add_custom_target(${uniqueTargetName} ALL DEPENDS ${_gmoFiles})
   else()
      add_custom_target(${uniqueTargetName} DEPENDS ${_gmoFiles})
   endif()

   add_dependencies(pofiles ${uniqueTargetName})

endfunction()

function(_KI18N_GETTEXT_GET_UNIQUE_TARGET_NAME _name _unique_name)
   set(propertyName "_KI18N_GETTEXT_UNIQUE_COUNTER_${_name}")
   get_property(currentCounter GLOBAL PROPERTY "${propertyName}")
   if(NOT currentCounter)
      set(currentCounter 1)
   endif()
   set(${_unique_name} "${_name}_${currentCounter}" PARENT_SCOPE)
   math(EXPR currentCounter "${currentCounter} + 1")
   set_property(GLOBAL PROPERTY ${propertyName} ${currentCounter} )
endfunction()
# End of CMake copied code ####################################################
