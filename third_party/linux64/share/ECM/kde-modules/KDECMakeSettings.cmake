#.rst:
# KDECMakeSettings
# ----------------
#
# Changes various CMake settings to what the KDE community views as more
# sensible defaults.
#
# It is recommended to include this module with the NO_POLICY_SCOPE flag,
# otherwise you may get spurious warnings with some versions of CMake.
#
# It is split into three parts, which can be independently disabled if desired.
#
# Runtime Paths
# ~~~~~~~~~~~~~
#
# The default runtime path (used on Unix systems to search for
# dynamically-linked libraries) is set to include the location that libraries
# will be installed to (as set in LIB_INSTALL_DIR or, if the former is not set,
# KDE_INSTALL_LIBDIR), and also the linker search path.
#
# Note that ``LIB_INSTALL_DIR`` or alternatively ``KDE_INSTALL_LIBDIR`` needs
# to be set before including this module.
# Typically, this is done by including the :kde-module:`KDEInstallDirs` module.
#
# This section can be disabled by setting ``KDE_SKIP_RPATH_SETTINGS`` to TRUE
# before including this module.
#
#
# Testing
# ~~~~~~~
#
# Testing is enabled by default, and an option (BUILD_TESTING) is provided for
# users to control this. See the CTest module documentation in the CMake manual
# for more details.
#
# This section can be disabled by setting ``KDE_SKIP_TEST_SETTINGS`` to TRUE
# before including this module.
#
#
# Build Settings
# ~~~~~~~~~~~~~~
#
# Various CMake build defaults are altered, such as searching source and build
# directories for includes first and enabling automoc by default.
#
# This section can be disabled by setting ``KDE_SKIP_BUILD_SETTINGS`` to TRUE
# before including this module.
#
# This section also provides an "uninstall" target that can be individually
# disabled by setting ``KDE_SKIP_UNINSTALL_TARGET`` to TRUE before including
# this module.
#
# By default on OS X, X11 and XCB related detections are disabled. However if
# the need would arise to use these technologies, the detection can be enabled
# by setting ``APPLE_FORCE_X11`` to ``ON``.
#
# A warning is printed for the developer to know that the detection is disabled on OS X.
# This message can be turned off by setting ``APPLE_SUPPRESS_X11_WARNING`` to ``ON``.
#
# Since pre-1.0.0.
#
# ``ENABLE_CLAZY`` option is added (OFF by default) when clang is being used.
# Turning this option on will force clang to load the clazy plugins for richer
# warnings on Qt-related code.
#
# If clang is not being used, this won't have an effect.
# See https://commits.kde.org/clazy?path=README.md
#
# Since 5.17.0
#
# - Uninstall target functionality since 1.7.0.
# - ``APPLE_FORCE_X11`` option since 5.14.0 (detecting X11 was previously the default behavior)
# - ``APPLE_SUPPRESS_X11_WARNING`` option since 5.14.0

#=============================================================================
# Copyright 2014      Alex Merry <alex.merry@kde.org>
# Copyright 2013      Aleix Pol <aleixpol@kde.org>
# Copyright 2012-2013 Stephen Kelly <steveire@gmail.com>
# Copyright 2007      Matthias Kretz <kretz@kde.org>
# Copyright 2006-2007 Laurent Montel <montel@kde.org>
# Copyright 2006-2013 Alex Neundorf <neundorf@kde.org>
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

################# RPATH handling ##################################

if(NOT KDE_SKIP_RPATH_SETTINGS)

   # Set the default RPATH to point to useful locations, namely where the
   # libraries will be installed and the linker search path

   # First look for the old LIB_INSTALL_DIR, then fallback to newer KDE_INSTALL_LIBDIR
   if(NOT LIB_INSTALL_DIR)
      if(NOT KDE_INSTALL_LIBDIR)
         message(FATAL_ERROR "Neither KDE_INSTALL_LIBDIR nor LIB_INSTALL_DIR is set. Setting one is necessary for using the RPATH settings.")
      else()
         set(_abs_LIB_INSTALL_DIR "${KDE_INSTALL_LIBDIR}")
      endif()
   else()
      set(_abs_LIB_INSTALL_DIR "${LIB_INSTALL_DIR}")
   endif()

   if (NOT IS_ABSOLUTE "${_abs_LIB_INSTALL_DIR}")
      set(_abs_LIB_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}")
   endif()

   if (UNIX)
      # for mac os: add install name dir in addition
      # check: is the rpath stuff below really required on mac os? at least it seems so to use a stock qt from qt.io
      if (APPLE)
         set(CMAKE_INSTALL_NAME_DIR ${_abs_LIB_INSTALL_DIR})
      endif ()

      # add our LIB_INSTALL_DIR to the RPATH (but only when it is not one of
      # the standard system link directories - such as /usr/lib on UNIX)
      list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${_abs_LIB_INSTALL_DIR}" _isSystemLibDir)
      list(FIND CMAKE_CXX_IMPLICIT_LINK_DIRECTORIES      "${_abs_LIB_INSTALL_DIR}" _isSystemCxxLibDir)
      list(FIND CMAKE_C_IMPLICIT_LINK_DIRECTORIES        "${_abs_LIB_INSTALL_DIR}" _isSystemCLibDir)
      if("${_isSystemLibDir}" STREQUAL "-1"  AND  "${_isSystemCxxLibDir}" STREQUAL "-1"  AND  "${_isSystemCLibDir}" STREQUAL "-1")
         set(CMAKE_INSTALL_RPATH "${_abs_LIB_INSTALL_DIR}")
      endif()

      # Append directories in the linker search path (but outside the project)
      set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
   endif (UNIX)

endif()

################ Testing setup ####################################

find_program(APPSTREAMCLI appstreamcli)
function(appstreamtest)
    if(APPSTREAMCLI AND NOT appstreamtest_added)
        set(appstreamtest_added TRUE PARENT_SCOPE)
        add_test(NAME appstreamtest COMMAND ${CMAKE_COMMAND} -DAPPSTREAMCLI=${APPSTREAMCLI} -DINSTALL_FILES=${CMAKE_BINARY_DIR}/install_manifest.txt -P ${CMAKE_CURRENT_LIST_DIR}/appstreamtest.cmake)
    else()
        message(STATUS "Could not set up the appstream test. appstreamcli is missing.")
    endif()
endfunction()

if(NOT KDE_SKIP_TEST_SETTINGS)

   # If there is a CTestConfig.cmake, include CTest.
   # Otherwise, there will not be any useful settings, so just
   # fake the functionality we care about from CTest.

   if (EXISTS ${CMAKE_SOURCE_DIR}/CTestConfig.cmake)
      include(CTest)
   else()
      option(BUILD_TESTING "Build the testing tree." ON)
      if(BUILD_TESTING)
         enable_testing()
         appstreamtest()
      endif ()
   endif ()

endif()



################ Build-related settings ###########################

if(NOT KDE_SKIP_BUILD_SETTINGS)

   # Always include srcdir and builddir in include path
   # This saves typing ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} in about every subdir
   # since cmake 2.4.0
   set(CMAKE_INCLUDE_CURRENT_DIR ON)

   # put the include dirs which are in the source or build tree
   # before all other include dirs, so the headers in the sources
   # are prefered over the already installed ones
   # since cmake 2.4.1
   set(CMAKE_INCLUDE_DIRECTORIES_PROJECT_BEFORE ON)

   # Add the src and build dir to the BUILD_INTERFACE include directories
   # of all targets. Similar to CMAKE_INCLUDE_CURRENT_DIR, but transitive.
   # Since CMake 2.8.11
   set(CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON)

   # When a shared library changes, but its includes do not, don't relink
   # all dependencies. It is not needed.
   # Since CMake 2.8.11
   set(CMAKE_LINK_DEPENDS_NO_SHARED ON)

   # Default to shared libs for KDE, if no type is explicitely given to add_library():
   set(BUILD_SHARED_LIBS TRUE CACHE BOOL "If enabled, shared libs will be built by default, otherwise static libs")

   # Enable automoc in cmake
   # Since CMake 2.8.6
   set(CMAKE_AUTOMOC ON)

   # By default, create 'GUI' executables. This can be reverted on a per-target basis
   # using ECMMarkNonGuiExecutable
   # Since CMake 2.8.8
   set(CMAKE_WIN32_EXECUTABLE ON)
   set(CMAKE_MACOSX_BUNDLE ON)

   # By default, don't put a prefix on MODULE targets. add_library(MODULE) is basically for plugin targets,
   # and in KDE plugins don't have a prefix.
   set(CMAKE_SHARED_MODULE_PREFIX "")

   unset(EXECUTABLE_OUTPUT_PATH)
   unset(LIBRARY_OUTPUT_PATH)
   unset(CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
   unset(CMAKE_LIBRARY_OUTPUT_DIRECTORY)
   unset(CMAKE_RUNTIME_OUTPUT_DIRECTORY)

   # under Windows, output all executables and dlls into
   # one common directory, and all static|import libraries and plugins
   # into another one. This way test executables can find their dlls
   # even without installation.
   if(WIN32)
      set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
      set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
      set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
   endif()

   # Disable detection of X11 and related package on OS X because when using
   # brew or macports, X11 can be installed and thus is detected.
   option(APPLE_FORCE_X11 "Force enable X11 related detection on OS X" OFF)
   option(APPLE_SUPPRESS_X11_WARNING "Suppress X11 and related technologies search disabling warning on OS X" OFF)

   if(APPLE AND NOT APPLE_FORCE_X11)
      if (NOT APPLE_SUPPRESS_X11_WARNING)
         message(WARNING "Searching for X11 and related technologies is disabled on Apple systems. Set APPLE_FORCE_X11 to ON to change this behaviour. Set APPLE_SUPPRESS_X11_WARNING to ON to hide this warning.")
      endif()

      set(CMAKE_DISABLE_FIND_PACKAGE_X11 true)
      set(CMAKE_DISABLE_FIND_PACKAGE_XCB true)
      set(CMAKE_DISABLE_FIND_PACKAGE_Qt5X11Extras true)
   endif()

   option(KDE_SKIP_UNINSTALL_TARGET "Prevent an \"uninstall\" target from being generated." OFF)
   if(NOT KDE_SKIP_UNINSTALL_TARGET)
       include("${ECM_MODULE_DIR}/ECMUninstallTarget.cmake")
   endif()

endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    option(ENABLE_CLAZY "Enable Clazy warnings" OFF)

    if(ENABLE_CLAZY)
        set(CMAKE_CXX_COMPILE_OBJECT "${CMAKE_CXX_COMPILE_OBJECT} -Xclang -load -Xclang ClangLazy.so -Xclang -add-plugin -Xclang clang-lazy")
    endif()
endif()

###################################################################
