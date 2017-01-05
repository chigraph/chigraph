#  KCONFIG_ADD_KCFG_FILES (SRCS_VAR [GENERATE_MOC] [USE_RELATIVE_PATH] file1.kcfgc ... fileN.kcfgc)
#    Use this to add KDE config compiler files to your application/library.
#    Use optional GENERATE_MOC to generate moc if you use signals in your kcfg files.
#    Use optional USE_RELATIVE_PATH to generate the classes in the build following the given
#    relative path to the file.
#
# Copyright (c) 2006-2009 Alexander Neundorf, <neundorf@kde.org>
# Copyright (c) 2006, 2007, Laurent Montel, <montel@kde.org>
# Copyright (c) 2007 Matthias Kretz <kretz@kde.org>
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

function (KCONFIG_ADD_KCFG_FILES _sources )
   set(options GENERATE_MOC USE_RELATIVE_PATH)
   cmake_parse_arguments(ARG "${options}" "" "" ${ARGN})

   set(sources)
   foreach (_current_FILE ${ARG_UNPARSED_ARGUMENTS})
       get_filename_component(_tmp_FILE ${_current_FILE} ABSOLUTE)
       get_filename_component(_abs_PATH ${_tmp_FILE} PATH)

       if (ARG_USE_RELATIVE_PATH) # Process relative path only if the option was set
           # Get relative path
           get_filename_component(_rel_PATH ${_current_FILE} PATH)

           if (IS_ABSOLUTE ${_rel_PATH})
               # We got an absolute path
               set(_rel_PATH "")
           endif ()
       endif ()

       get_filename_component(_basename ${_tmp_FILE} NAME_WE)
       # If we had a relative path and we're asked to use it, then change the basename accordingly
       if(_rel_PATH)
           set(_basename ${_rel_PATH}/${_basename})
       endif()

       file(READ ${_tmp_FILE} _contents)
       string(REGEX MATCH "File=([^\n]+\\.kcfg)\n" "" "${_contents}")
       set(_kcfg_FILENAME "${CMAKE_MATCH_1}")
       if (NOT _kcfg_FILENAME)
            string(REGEX MATCH "File=([^\n]+\\.kcfg).*\n" "" "${_contents}")
            if(CMAKE_MATCH_1)
                message(WARNING "${_tmp_FILE}: Broken \"File\" field, make sure it's pointing at the right file")
                set(_kcfg_FILENAME "${CMAKE_MATCH_1}")
            else()
                message(WARNING "Couldn't read the \"File\" field in ${_tmp_FILE}")
                set(_kcfg_FILENAME "${_basename}.kcfg")
            endif()
       endif()
       set(_src_FILE    ${CMAKE_CURRENT_BINARY_DIR}/${_basename}.cpp)
       set(_header_FILE ${CMAKE_CURRENT_BINARY_DIR}/${_basename}.h)
       set(_moc_FILE    ${CMAKE_CURRENT_BINARY_DIR}/${_basename}.moc)
       set(_kcfg_FILE   ${_abs_PATH}/${_kcfg_FILENAME})
       # Maybe the .kcfg is a generated file?
       if(NOT EXISTS "${_kcfg_FILE}")
           set(_kcfg_FILE   ${CMAKE_CURRENT_BINARY_DIR}/${_kcfg_FILENAME})
       endif()

       if(NOT EXISTS "${_kcfg_FILE}")
           message(FATAL_ERROR "${_kcfg_FILENAME} not found; tried in ${_abs_PATH} and ${CMAKE_CURRENT_BINARY_DIR}")
       endif()

       # make sure the directory exist in the build directory
       if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/${_rel_PATH}")
           file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${_rel_PATH})
       endif()

       # the command for creating the source file from the kcfg file
       add_custom_command(OUTPUT ${_header_FILE} ${_src_FILE}
                          COMMAND KF5::kconfig_compiler
                          ARGS ${_kcfg_FILE} ${_tmp_FILE} -d ${CMAKE_CURRENT_BINARY_DIR}/${_rel_PATH}
                          MAIN_DEPENDENCY ${_tmp_FILE}
                          DEPENDS ${_kcfg_FILE})

       if(ARG_GENERATE_MOC)
          list(APPEND sources ${_moc_FILE})
          qt5_generate_moc(${_header_FILE} ${_moc_FILE})
          set_property(SOURCE ${_src_FILE} PROPERTY SKIP_AUTOMOC TRUE)  # don't run automoc on this file
          set_property(SOURCE ${_src_FILE} APPEND PROPERTY OBJECT_DEPENDS ${_moc_FILE} )
       endif()

       list(APPEND sources ${_src_FILE} ${_header_FILE})
   endforeach (_current_FILE)

   set(${_sources} ${${_sources}} ${sources} PARENT_SCOPE)

endfunction(KCONFIG_ADD_KCFG_FILES)
