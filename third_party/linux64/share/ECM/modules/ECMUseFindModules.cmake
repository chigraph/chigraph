#.rst:
# ECMUseFindModules
# -----------------
#
# Selectively use some of the find modules provided by extra-cmake-modules.
#
# This module is automatically available once extra-cmake-modules has been
# found, so it is not necessary to ``include(ECMUseFindModules)`` explicitly.
#
# ::
#
#   ecm_use_find_modules(DIR <dir>
#                        MODULES module1.cmake [module2.cmake [...]]
#                        [NO_OVERRIDE])
#
# This allows selective use of the find modules provided by ECM, including
# deferring to CMake's versions of those modules if it has them.  Rather than
# adding ``${ECM_FIND_MODULE_DIR}`` to ``CMAKE_MODULE_PATH``, you use
# ecm_use_find_modules() to copy the modules you want to a local (build)
# directory, and add that to ``CMAKE_MODULE_PATH``.
#
# The find modules given to MODULES will be copied to the directory given by DIR
# (which should be located in ``${CMAKE_BINARY_DIR}`` and added to
# ``CMAKE_MODULE_PATH``).  If NO_OVERRIDE is given, only modules not also
# provided by CMake will be copied.
#
# Example:
#
# .. code-block:: cmake
#
#   find_package(ECM REQUIRED)
#   ecm_use_find_modules(
#       DIR ${CMAKE_BINARY_DIR}/cmake
#       MODULES FindEGL.cmake
#       NO_OVERRIDE
#   )
#   set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR}/cmake)
#
# This example will make ``FindEGL.cmake`` available in your project, but only
# as long as it is not yet part of CMake. Calls to ``find_package(EGL)`` will
# then make use of this copied module (or the CMake module if it exists).
#
# Another possible use for this macro is to take copies of find modules that can
# be installed along with config files if they are required as a dependency (for
# example, if targets provided by the find module are in the link interface of a
# library).
#
# Since pre-1.0.0.

#=============================================================================
# Copyright 2011 Alexander Neundorf <neundorf@kde.org>
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

function(ecm_use_find_modules)
   set(_options NO_OVERRIDE )
   set(_oneValueArgs DIR )
   set(_multiValueArgs MODULES )
   cmake_parse_arguments(EUFM "${_options}" "${_oneValueArgs}" "${_multiValueArgs}" ${ARGN} )
   if(NOT EUFM_DIR)
      message(FATAL_ERROR "No DIR specified for ecm_use_find_modules() !")
   endif()

   if(NOT IS_ABSOLUTE "${EUFM_DIR}")
      set(EUFM_DIR "${CMAKE_CURRENT_BINARY_DIR}/${EUFM_DIR}")
   endif()
   file(MAKE_DIRECTORY "${EUFM_DIR}")

   foreach(file ${EUFM_MODULES})
      if(NOT EXISTS ${ECM_FIND_MODULE_DIR}/${file} )
         message(FATAL_ERROR "File ${file} not found in ${ECM_FIND_MODULE_DIR} !")
      endif()
      if(NOT EXISTS "${CMAKE_ROOT}/Modules/${file}" OR NOT EUFM_NO_OVERRIDE)
         configure_file("${ECM_FIND_MODULE_DIR}/${file}" "${EUFM_DIR}/${file}" COPYONLY)
      endif()
   endforeach()
   # This is required by some of the find modules
   file(WRITE "${EUFM_DIR}/ECMFindModuleHelpersStub.cmake"
        "include(\"${ECM_MODULE_DIR}/ECMFindModuleHelpers.cmake\")")

endfunction()
