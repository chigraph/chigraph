#.rst:
# FindKF5
# -------
#
# Find KDE Frameworks 5 with a single find_package() call.
#
# This will use the package config files provided by the individual frameworks.
# For example, if you wish to find KArchive, which presents itself to CMake as
# KF5Archive (ie: you would do ``find_package(KF5Archive)`` to find it
# directly), you can do
#
# .. code-block:: cmake
#
#   find_package(KF5 COMPONENTS Archive)
#
# If all the required components (those given in the COMPONENTS argument, but
# not those given in the OPTIONAL_COMPONENTS argument) are found, ``KF5_FOUND``
# will be set to true. Otherwise, it will be set to false.
#
# Since pre-1.0.0.

#=============================================================================
# Copyright 2014 Alex Merry <alex.merry@kde.org>
# Copyright 2013 Stephen Kelly <steveire@gmail.com>
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

ecm_find_package_version_check(KF5)

if (NOT KF5_FIND_COMPONENTS)
    set(KF5_NOT_FOUND_MESSAGE "The KF5 package requires at least one component")
    set(KF5_FOUND False)
    return()
endif()

set(_quiet_arg)
if (KF5_FIND_QUIETLY)
    set(_quiet_arg QUIET)
endif()
set(_exact_arg)
if (KF5_FIND_EXACT)
    set(_exact_arg EXACT)
endif()

include(FindPackageHandleStandardArgs)
include(FeatureSummary)

set(KF5_VERSION)
foreach(_module ${KF5_FIND_COMPONENTS})
    find_package(KF5${_module} ${KF5_FIND_VERSION}
        ${_exact_arg} ${_quiet_arg}
        CONFIG
    )
    find_package_handle_standard_args(KF5${_module} CONFIG_MODE)
    if (KF5_FIND_REQUIRED AND KF5_FIND_REQUIRED_${_module})
        # If the component was required, we tell FeatureSummary so that it
        # will be displayed in the correct list. We do not use the REQUIRED
        # argument of find_package() to allow all the missing frameworks
        # to be listed at once (fphsa will error out at the end of this file
        # anyway).
        set_package_properties(KF5${_module} PROPERTIES TYPE REQUIRED)
    endif()

    # Component-based find modules are expected to set
    # <module>_<component>_FOUND and <module>_<component>_VERSION variables,
    # but the find_package calls above will have set KF5<component>_*
    # variables.
    set(KF5_${_module}_FOUND ${KF5${_module}_FOUND})
    if(KF5${_module}_FOUND)
        set(KF5_${_module}_VERSION ${KF5${_module}_VERSION})

        # make KF5_VERSION the minimum found version
        if(NOT KF5_VERSION OR KF5_VERSION VERSION_GREATER KF5${_module}_VERSION)
            set(KF5_VERSION ${KF5${_module}_VERSION})
        endif()
    endif()
endforeach()

# Annoyingly, find_package_handle_standard_args requires you to provide
# REQUIRED_VARS even when using HANDLE_COMPONENTS, but all we actually
# care about is whether the required components were found. So we provide
# a dummy variable that is just set to something that will be printed
# on success.
set(_dummy_req_var "success")

find_package_handle_standard_args(KF5
    FOUND_VAR
        KF5_FOUND
    REQUIRED_VARS
        _dummy_req_var
    VERSION_VAR
        KF5_VERSION
    HANDLE_COMPONENTS
)

