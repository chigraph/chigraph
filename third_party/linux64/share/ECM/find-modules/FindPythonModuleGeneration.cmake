#.rst:
# FindPythonModuleGeneration
# --------------------------
#
# This module is experimental and internal.  The interface will likely
# change in the coming releases.
#
# Tools and macros for generating python bindings
#
# This will define the following public function:
#
#   ecm_generate_python_binding(TARGET <target>
#                               PYTHONNAMESPACE <namespace>
#                               MODULENAME <modulename>
#                               HEADERS <headers>)
#
# Invoking the function will create bindings for the <target> for python 2 and 3,
# if available.  The bindings will be put in the namespace <namespace> in python,
# and will be available from the module <modulename>.
#
# The optional rules file specifies the rules for creating the bindings
#
# A simple invokation would be:
#
#   ecm_generate_python_binding(KMyTarget
#     PYTHONNAMESPACE PyKF5
#     MODULENAME MyTarget
#     SIP_DEPENDS QtCore/QtCoremod.sip
#     HEADERS ${myTargetHeaders}
#   )
#
# which can then be used from python as
#
#   import PyKF5.MyTarget
#

#=============================================================================
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


macro(_find_python version minor_version)
  set(_CURRENT_VERSION ${version}.${minor_version})
  find_library(GPB_PYTHON${version}_LIBRARY
    NAMES
      python${_CURRENT_VERSION}mu
      python${_CURRENT_VERSION}m
      python${_CURRENT_VERSION}u
      python${_CURRENT_VERSION}
    PATHS
      [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\${_CURRENT_VERSION}\\InstallPath]/libs
      [HKEY_CURRENT_USER\\SOFTWARE\\Python\\PythonCore\\${_CURRENT_VERSION}\\InstallPath]/libs
    # Avoid finding the .dll in the PATH.  We want the .lib.
    NO_SYSTEM_ENVIRONMENT_PATH
  )

  if(GPB_PYTHON${version}_LIBRARY)
    # Use the library's install prefix as a hint
    set(_Python_INCLUDE_PATH_HINT)
    get_filename_component(_Python_PREFIX ${GPB_PYTHON${version}_LIBRARY} PATH)
    get_filename_component(_Python_PREFIX ${_Python_PREFIX} PATH)
    if(_Python_PREFIX)
      set(_Python_INCLUDE_PATH_HINT ${_Python_PREFIX}/include)
    endif()
    unset(_Python_PREFIX)

    find_path(GPB_PYTHON${version}_INCLUDE_DIR
      NAMES Python.h
      HINTS
        ${_Python_INCLUDE_PATH_HINT}
      PATHS
        [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\${_CURRENT_VERSION}\\InstallPath]/include
        [HKEY_CURRENT_USER\\SOFTWARE\\Python\\PythonCore\\${_CURRENT_VERSION}\\InstallPath]/include
      PATH_SUFFIXES
        python${_CURRENT_VERSION}mu
        python${_CURRENT_VERSION}m
        python${_CURRENT_VERSION}u
        python${_CURRENT_VERSION}
    )
  endif()

  find_program(GPB_PYTHON${version}_COMMAND python${version})
endmacro()

macro(_create_imported_python_target version)
  if(GPB_PYTHON${version}_LIBRARY AND GPB_PYTHON${version}_INCLUDE_DIR AND EXISTS "${GPB_PYTHON${version}_INCLUDE_DIR}/patchlevel.h")
    list(APPEND _pyversions ${version})

    file(STRINGS "${GPB_PYTHON${version}_INCLUDE_DIR}/patchlevel.h" python_version_define
         REGEX "^#define[ \t]+PY_MINOR_VERSION[ \t]+[0-9]+")
    string(REGEX REPLACE "^#define[ \t]+PY_MINOR_VERSION[ \t]+([0-9]+)" "\\1"
                         min_ver "${python_version_define}")
    unset(python_version_define)

    set(pyversion${version}_maj_min ${version}.${min_ver})

    add_library(Python::Libs${version} UNKNOWN IMPORTED)
    set_property(TARGET Python::Libs${version} PROPERTY IMPORTED_LOCATION ${GPB_PYTHON${version}_LIBRARY})
    set_property(TARGET Python::Libs${version} PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${GPB_PYTHON${version}_INCLUDE_DIR})
  endif()
endmacro()

macro(_report_NOT_FOUND message)
  if(NOT ${CMAKE_FIND_PACKAGE_NAME}_FIND_QUIETLY)
    set(_GPB_MESSAGE_TYPE STATUS)
    if(${CMAKE_FIND_PACKAGE_NAME}_FIND_REQUIRED)
      set(_GPB_MESSAGE_TYPE FATAL_ERROR)
    endif()
    message(${_GPB_MESSAGE_TYPE} ${message})
  endif()
  set(PythonModuleGeneration_FOUND FALSE)
  return()
endmacro()

if (NOT Qt5Core_FOUND)
  _report_NOT_FOUND("Qt 5 must be found before finding ${CMAKE_FIND_PACKAGE_NAME}.")
endif()

if (NOT GPB_PYTHON3_LIBRARY)
  set(_PYTHON3_MIN_VERSION 4)
  set(_PYTHON3_MAX_VERSION 10)

  _find_python(3 ${_PYTHON3_MAX_VERSION}) # Canary check

  if (GPB_PYTHON3_LIBRARY)
    message(FATAL_ERROR "The max python version in ${CMAKE_FIND_PACKAGE_NAME} must be updated.")
  endif()

  set(_PYTHON3_FIND_VERSION ${_PYTHON3_MAX_VERSION})

  while(NOT GPB_PYTHON3_LIBRARY
      AND NOT GPB_PYTHON3_INCLUDE_DIR
      AND NOT EXISTS "${GPB_PYTHON3_INCLUDE_DIR}/patchlevel.h"
      AND NOT _PYTHON3_FIND_VERSION EQUAL ${_PYTHON3_MIN_VERSION})
    math(EXPR _PYTHON3_FIND_VERSION "${_PYTHON3_FIND_VERSION} - 1")
    _find_python(3 ${_PYTHON3_FIND_VERSION})
  endwhile()
endif()
_create_imported_python_target(3)

_find_python(2 7)
_create_imported_python_target(2)

if (NOT _pyversions)
  _report_NOT_FOUND("At least one python version must be available to use ${CMAKE_FIND_PACKAGE_NAME}.")
endif()

find_program(GBP_SIP_COMMAND sip)

if (NOT GBP_SIP_COMMAND)
  _report_NOT_FOUND("The sip executable must be available to use ${CMAKE_FIND_PACKAGE_NAME}.")
endif()

if (NOT GPB_PYTHON2_COMMAND)
  _report_NOT_FOUND("The python2 executable is required by clang-python for the ${CMAKE_FIND_PACKAGE_NAME} Module.")
endif()

if (NOT libclang_LIBRARY)
  set(_LIBCLANG3_MAX_VERSION 16)
  set(_LIBCLANG3_MIN_VERSION 8)

  find_library(libclang_LIBRARY clang-3.${_LIBCLANG3_MAX_VERSION})

  if (libclang_LIBRARY)
    message(FATAL_ERROR "The max libclang-3 version in ${CMAKE_FIND_PACKAGE_NAME} must be updated.")
  endif()

  set(_LIBCLANG3_FIND_VERSION ${_LIBCLANG3_MAX_VERSION})
  while(NOT libclang_LIBRARY AND NOT _LIBCLANG3_FIND_VERSION EQUAL _LIBCLANG3_MIN_VERSION)
    math(EXPR _LIBCLANG3_FIND_VERSION "${_LIBCLANG3_FIND_VERSION} - 1")
    set(_GPB_CLANG_SUFFIX 3.${_LIBCLANG3_FIND_VERSION})
    find_library(libclang_LIBRARY clang-3.${_LIBCLANG3_FIND_VERSION})
  endwhile()
endif()

if (NOT libclang_LIBRARY)
  _report_NOT_FOUND("Could not find libclang version 3.8 or greater.")
endif()

execute_process(
  COMMAND ${GPB_PYTHON2_COMMAND} ${CMAKE_CURRENT_LIST_DIR}/sip_generator.py --self-check ${libclang_LIBRARY}
  RESULT_VARIABLE selfCheckErrors
)

if (selfCheckErrors)
  _report_NOT_FOUND("sip_generator failed a self-check for the ${CMAKE_FIND_PACKAGE_NAME} Module.")
endif()

get_filename_component(libclang_file "${libclang_file}" REALPATH)

find_file(SIP_Qt5Core_Mod_FILE
  NAMES QtCoremod.sip
  PATH_SUFFIXES share/sip/PyQt5/QtCore
)

if(NOT SIP_Qt5Core_Mod_FILE)
  _report_NOT_FOUND("PyQt module files not found for the ${CMAKE_FIND_PACKAGE_NAME} Module.")
endif()

file(STRINGS "${SIP_Qt5Core_Mod_FILE}" _SIP_Qt5_VERSIONS
  REGEX "^%Timeline"
)

string(REGEX MATCHALL "Qt_5_[^ }]+" _SIP_Qt5_VERSIONS "${_SIP_Qt5_VERSIONS}")

set(GPB_Qt5_Tag Qt_5_${Qt5Core_VERSION_MINOR}_${Qt5Core_VERSION_PATCH})

list(FIND _SIP_Qt5_VERSIONS ${GPB_Qt5_Tag} _SIP_Qt5_Version_Index)

if(_SIP_Qt5_Version_Index EQUAL -1)
  _report_NOT_FOUND("PyQt module \"${SIP_Qt5Core_Mod_FILE}\" does not support Qt version 5.${Qt5Core_VERSION_MINOR}.${Qt5Core_VERSION_PATCH} for the ${CMAKE_FIND_PACKAGE_NAME} Module. Found available Qt5 tags: \"${_SIP_Qt5_VERSIONS}\".")
endif()

set(PythonModuleGeneration_FOUND TRUE)

include(CMakeParseArguments)

set(GPB_MODULE_DIR ${CMAKE_CURRENT_LIST_DIR})

function(_compute_implicit_include_dirs)
  set(CLANG_CXX_DRIVER clang++${_GPB_CLANG_SUFFIX})
  if (NOT EXISTS ${CLANG_CXX_DRIVER})
    set(CLANG_CXX_DRIVER clang++)
  endif()
  execute_process(COMMAND ${CLANG_CXX_DRIVER} -v -E -x c++ -
                  ERROR_VARIABLE _compilerOutput
                  OUTPUT_VARIABLE _compilerStdout
                  INPUT_FILE /dev/null)

  if( "${_compilerOutput}" MATCHES "> search starts here[^\n]+\n *(.+ *\n) *End of (search) list" )

    # split the output into lines and then remove leading and trailing spaces from each of them:
    string(REGEX MATCHALL "[^\n]+\n" _includeLines "${CMAKE_MATCH_1}")
    foreach(nextLine ${_includeLines})
      # on OSX, gcc says things like this:  "/System/Library/Frameworks (framework directory)", strip the last part
      string(REGEX REPLACE "\\(framework directory\\)" "" nextLineNoFramework "${nextLine}")
      # strip spaces at the beginning and the end
      string(STRIP "${nextLineNoFramework}" _includePath)
      list(APPEND _resultIncludeDirs "${_includePath}")
    endforeach()
  endif()

  set(_GPB_IMPLICIT_INCLUDE_DIRS ${_resultIncludeDirs} PARENT_SCOPE)
endfunction()

function(ecm_generate_python_binding
    target_keyword target_value
    pythonnamespace_keyword pythonnamespace_value
    modulename_keyword modulename_value
    )

    cmake_parse_arguments(GPB "" "RULES_FILE" "SIP_DEPENDS;SIP_INCLUDES;HEADERS"  ${ARGN})

    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/sip/${pythonnamespace_value}/${modulename_value}/${modulename_value}mod.sip"
          "
%Module ${pythonnamespace_value}.${modulename_value}

%ModuleHeaderCode
#pragma GCC visibility push(default)
%End\n\n")

    set(generator_depends "${GPB_MODULE_DIR}/sip_generator.py"
        "${GPB_MODULE_DIR}/rules_engine.py"
        "${GPB_MODULE_DIR}/FindPythonModuleGeneration.cmake"
        )

    if (NOT _GPB_IMPLICIT_INCLUDE_DIRS)
      _compute_implicit_include_dirs()
    endif()

    foreach(dep ${GPB_SIP_DEPENDS})
        if (IS_ABSOLUTE ${dep})
          list(APPEND generator_depends "${dep}")
        endif()
        file(APPEND "${CMAKE_CURRENT_BINARY_DIR}/sip/${pythonnamespace_value}/${modulename_value}/${modulename_value}mod.sip"
          "%Import ${dep}\n\n")
    endforeach()

    set(sip_files)
    set(commands)

    if (NOT GPB_RULES_FILE)
      set(GPB_RULES_FILE "${GPB_MODULE_DIR}/Qt5Ruleset.py")
    endif()


    list(APPEND generator_depends ${GPB_RULES_FILE})

    foreach(hdr ${GPB_HEADERS})

        get_filename_component(hdr_file ${hdr} ABSOLUTE)

        get_filename_component(hdr ${hdr} NAME_WE)
        string(TOLOWER ${hdr}.h hdr_filename)

        if (NOT EXISTS "${hdr_file}")
          message(FATAL_ERROR "File not found: ${hdr_file}")
        endif()

        set(sip_file "${CMAKE_CURRENT_BINARY_DIR}/sip/${pythonnamespace_value}/${modulename_value}/${hdr}.sip")
        list(APPEND sip_files ${sip_file})

        set(inc_dirs "-I$<JOIN:$<TARGET_PROPERTY:${target_value},INTERFACE_INCLUDE_DIRECTORIES>,;-I>")
        set(sys_inc_dirs)
        foreach(d ${_GPB_IMPLICIT_INCLUDE_DIRS})
           list(APPEND sys_inc_dirs "-isystem" "${d}")
        endforeach()
        set(comp_defs "-D$<JOIN:$<TARGET_PROPERTY:${target_value},INTERFACE_COMPILE_DEFINITIONS>,;-D>")

        foreach(stdVar 11 14)
          set(stdFlag "$<$<STREQUAL:$<TARGET_PROPERTY:${target_value},CXX_STANDARD>,${stdVar}>:${CMAKE_CXX${stdVar}_EXTENSION_COMPILE_OPTION}>")
        endforeach()

        set(comp_flags "$<JOIN:$<TARGET_PROPERTY:${target_value},INTERFACE_COMPILE_OPTIONS>;${stdFlag},;>")

        add_custom_command(OUTPUT ${sip_file}
            COMMAND ${GPB_PYTHON2_COMMAND} ${GPB_MODULE_DIR}/sip_generator.py
              --flags " ${inc_dirs};${sys_inc_dirs};${comp_defs};${comp_flags}"
              --include_filename "${hdr_filename}"
              ${libclang_LIBRARY}
              ${GPB_RULES_FILE}
              "${hdr_file}"
              "${sip_file}"
            DEPENDS ${hdr_file} ${generator_depends}
            VERBATIM
        )

        file(APPEND "${CMAKE_CURRENT_BINARY_DIR}/sip/${pythonnamespace_value}/${modulename_value}/${modulename_value}mod.sip"
          "%Include ${hdr}.sip\n")
    endforeach()

    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/pybuild/${pythonnamespace_value}/${modulename_value}/module.sbf"
        "
target = ${modulename_value}
sources = sip${modulename_value}cmodule.cpp
headers = sipAPI${modulename_value}
"
    )

    get_filename_component(SIP_PYQT5_DIR ${SIP_Qt5Core_Mod_FILE} PATH)
    get_filename_component(SIP_PYQT5_DIR ${SIP_PYQT5_DIR} PATH)

    set(sip_includes -I "${SIP_PYQT5_DIR}")
    if (GPB_SIP_INCLUDES)
      list(APPEND sip_includes -I "$<JOIN:${GPB_SIP_INCLUDES},-I>")
    endif()
    foreach(path ${CMAKE_PREFIX_PATH} ${CMAKE_INSTALL_PREFIX})
      if (EXISTS ${path}/share/sip)
        list(APPEND sip_includes -I "${path}/share/sip")
      endif()
    endforeach()

    if (WIN32)
      set(GPB_WS_Tag -t WS_WIN)
    elif(APPLE)
      set(GPB_WS_Tag -t WS_MACX)
    else()
      set(GPB_WS_Tag -t WS_X11)
    endif()

    add_custom_target(generate_${modulename_value}_sip_files ALL DEPENDS ${sip_files})

    add_custom_command(OUTPUT
      "${CMAKE_CURRENT_BINARY_DIR}/pybuild/${pythonnamespace_value}/${modulename_value}/unified${modulename_value}.cpp"
      COMMAND ${GPB_PYTHON2_COMMAND} "${GPB_MODULE_DIR}/run-sip.py" --sip ${GBP_SIP_COMMAND}
       --unify "${CMAKE_CURRENT_BINARY_DIR}/pybuild/${pythonnamespace_value}/${modulename_value}/unified${modulename_value}.cpp"
       --module-name "${modulename_value}"
       -c "${CMAKE_CURRENT_BINARY_DIR}/pybuild/${pythonnamespace_value}/${modulename_value}"
       -b "${CMAKE_CURRENT_BINARY_DIR}/pybuild/${pythonnamespace_value}/${modulename_value}/module.sbf"
       -t ${GPB_Qt5_Tag} ${GPB_WS_Tag}

       -x VendorID -x Py_v3

       -I "${CMAKE_CURRENT_BINARY_DIR}/sip/${pythonnamespace_value}/${modulename_value}"
       ${sip_includes}
       "${CMAKE_CURRENT_BINARY_DIR}/sip/${pythonnamespace_value}/${modulename_value}/${modulename_value}mod.sip"
       DEPENDS generate_${modulename_value}_sip_files "${GPB_MODULE_DIR}/run-sip.py" ${generator_depends}
    )

    add_custom_target(sip_generated_${modulename_value}_files ALL
          DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/pybuild/${pythonnamespace_value}/${modulename_value}/unified${modulename_value}.cpp")

    file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/sip/${pythonnamespace_value}/${modulename_value}"
         "${CMAKE_CURRENT_BINARY_DIR}/pybuild/${pythonnamespace_value}/${modulename_value}")

    foreach(pyversion ${_pyversions})
        file(MAKE_DIRECTORY
            "${CMAKE_CURRENT_BINARY_DIR}/py${pyversion}/${pythonnamespace_value}")
        execute_process(COMMAND "${CMAKE_COMMAND}" -E touch "${CMAKE_CURRENT_BINARY_DIR}/py${pyversion}/${pythonnamespace_value}/__init__.py")

        add_library(Py${pyversion}KF5${modulename_value} MODULE
          "${CMAKE_CURRENT_BINARY_DIR}/pybuild/${pythonnamespace_value}/${modulename_value}/unified${modulename_value}.cpp"
        )
        add_dependencies(Py${pyversion}KF5${modulename_value} sip_generated_${modulename_value}_files)
        target_link_libraries(Py${pyversion}KF5${modulename_value} PRIVATE ${target_value} Python::Libs${pyversion})

        target_compile_options(Py${pyversion}KF5${modulename_value} PRIVATE -fstack-protector-strong -Wno-deprecated-declarations -Wno-overloaded-virtual)
        target_include_directories(Py${pyversion}KF5${modulename_value} PRIVATE ${GPB_SIP_INCLUDES})
        target_link_libraries(Py${pyversion}KF5${modulename_value} PRIVATE -Wl,-Bsymbolic-functions -Wl,-z,relro)

        set_property(TARGET Py${pyversion}KF5${modulename_value} PROPERTY AUTOMOC OFF)
        set_property(TARGET Py${pyversion}KF5${modulename_value} PROPERTY PREFIX "")
        set_property(TARGET Py${pyversion}KF5${modulename_value} PROPERTY OUTPUT_NAME py${pyversion}/${pythonnamespace_value}/${modulename_value})

        add_test(NAME Py${pyversion}Test COMMAND ${GPB_PYTHON${pyversion}_COMMAND} "${CMAKE_SOURCE_DIR}/autotests/pythontest.py" ${CMAKE_CURRENT_BINARY_DIR}/py${pyversion})

        install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/py${pyversion}/${pythonnamespace_value}
            DESTINATION lib/python${pyversion${pyversion}_maj_min}/dist-packages)
        install(FILES ${sip_files} "${CMAKE_CURRENT_BINARY_DIR}/sip/${pythonnamespace_value}/${modulename_value}/${modulename_value}mod.sip"
          DESTINATION share/sip/${pythonnamespace_value}/${modulename_value}
        )
    endforeach()
endfunction()
