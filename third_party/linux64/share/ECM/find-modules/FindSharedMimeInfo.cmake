#.rst:
# FindSharedMimeInfo
# ------------------
#
# Try to find the shared-mime-info package.
#
# This will define the following variables:
#
# ``SharedMimeInfo_FOUND``
#     True if system has the shared-mime-info package
# ``UPDATE_MIME_DATABASE_EXECUTABLE``
#     The update-mime-database executable
#
# and the following imported targets:
#
# ``SharedMimeInfo::UpdateMimeDatabase``
#     The update-mime-database executable
#
# The follow macro is available::
#
#   update_xdg_mimetypes(<path>)
#
# Updates the XDG mime database at install time (unless the ``$DESTDIR``
# environment variable is set, in which case it is up to package managers to
# perform this task).
#
# Since pre-1.0.0.

#=============================================================================
# Copyright 2013-2014 Alex Merry <alex.merry@kdemail.net>
# Copyright 2007 Pino Toscano <toscano.pino@tiscali.it>
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

ecm_find_package_version_check(SharedMimeInfo)

find_program (UPDATE_MIME_DATABASE_EXECUTABLE NAMES update-mime-database)

if (UPDATE_MIME_DATABASE_EXECUTABLE)
    execute_process(
        COMMAND "${UPDATE_MIME_DATABASE_EXECUTABLE}" -v
        OUTPUT_VARIABLE _smiVersionRaw
        ERROR_VARIABLE _smiVersionRaw)

    string(REGEX REPLACE "update-mime-database \\([a-zA-Z\\-]+\\) ([0-9]\\.[0-9]+).*"
           "\\1" SharedMimeInfo_VERSION_STRING "${_smiVersionRaw}")
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SharedMimeInfo
    FOUND_VAR
        SharedMimeInfo_FOUND
    REQUIRED_VARS
        UPDATE_MIME_DATABASE_EXECUTABLE
    VERSION_VAR
        SharedMimeInfo_VERSION_STRING)

if(SharedMimeInfo_FOUND AND NOT TARGET SharedMimeInfo::UpdateMimeDatabase)
    add_executable(SharedMimeInfo::UpdateMimeDatabase IMPORTED)
    set_target_properties(SharedMimeInfo::UpdateMimeDatabase PROPERTIES
        IMPORTED_LOCATION "${UPDATE_MIME_DATABASE_EXECUTABLE}"
    )
endif()

mark_as_advanced(UPDATE_MIME_DATABASE_EXECUTABLE)

function(UPDATE_XDG_MIMETYPES _path)
    get_filename_component(_xdgmimeDir "${_path}" NAME)
    if("${_xdgmimeDir}" STREQUAL packages )
        get_filename_component(_xdgmimeDir "${_path}" PATH)
    else()
        set(_xdgmimeDir "${_path}")
    endif()

    # Note that targets and most variables are not available to install code
    install(CODE "
set(DESTDIR_VALUE \"\$ENV{DESTDIR}\")
if (NOT DESTDIR_VALUE)
    # under Windows relative paths are used, that's why it runs from CMAKE_INSTALL_PREFIX
    message(STATUS \"Updating MIME database at \${CMAKE_INSTALL_PREFIX}/${_xdgmimeDir}\")
    execute_process(COMMAND \"${UPDATE_MIME_DATABASE_EXECUTABLE}\" \"${_xdgmimeDir}\"
                    WORKING_DIRECTORY \"\${CMAKE_INSTALL_PREFIX}\")
endif (NOT DESTDIR_VALUE)
")
endfunction()

include(FeatureSummary)
set_package_properties(SharedMimeInfo PROPERTIES
    URL http://freedesktop.org/wiki/Software/shared-mime-info/
    DESCRIPTION "A database of common MIME types")
