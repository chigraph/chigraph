#.rst:
# ECMUninstallTarget
# ------------------
#
# Add an ``uninstall`` target.
#
# By including this module, an ``uninstall`` target will be added to your CMake
# project. This will remove all files installed (or updated) by a previous
# invocation of the ``install`` target. It will not remove files created or
# modified by an ``install(SCRIPT)`` or ``install(CODE)`` command; you should
# create a custom uninstallation target for these and use ``add_dependency`` to
# make the ``uninstall`` target depend on it:
#
# .. code-block:: cmake
#
#   include(ECMUninstallTarget)
#   install(SCRIPT install-foo.cmake)
#   add_custom_target(uninstall_foo COMMAND ${CMAKE_COMMAND} -P uninstall-foo.cmake)
#   add_dependency(uninstall uninstall_foo)
#
# The target will fail if the ``install`` target has not yet been run (so it is
# not possible to run CMake on the project and then immediately run the
# ``uninstall`` target).
#
# .. warning::
#
#   CMake deliberately does not provide an ``uninstall`` target by default on
#   the basis that such a target has the potential to remove important files
#   from a user's computer. Use with caution.
#
# Since 1.7.0.

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

if (NOT TARGET uninstall)
    configure_file(
        "${CMAKE_CURRENT_LIST_DIR}/ecm_uninstall.cmake.in"
        "${CMAKE_BINARY_DIR}/ecm_uninstall.cmake"
        IMMEDIATE
        @ONLY
    )

    add_custom_target(uninstall
        COMMAND "${CMAKE_COMMAND}" -P "${CMAKE_BINARY_DIR}/ecm_uninstall.cmake"
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
    )
endif()
