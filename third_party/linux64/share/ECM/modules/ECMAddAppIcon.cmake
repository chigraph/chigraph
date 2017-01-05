#.rst:
# ECMAddAppIcon
# -------------
#
# Add icons to executable files and packages.
#
# ::
#
#  ecm_add_app_icon(<sources_var>
#                   ICONS <icon> [<icon> [...]])
#
# The given icons, whose names must match the pattern::
#
#   <size>-<other_text>.png
#
# will be added to the executable target whose sources are specified by
# ``<sources_var>`` on platforms that support it (Windows and Mac OS X).
#
# ``<size>`` is a numeric pixel size (typically 16, 32, 48, 64, 128 or 256).
# ``<other_text>`` can be any other text. See the platform notes below for any
# recommendations about icon sizes.
#
# Windows notes
#    * Icons are compiled into the executable using a resource file.
#    * Icons may not show up in Windows Explorer if the executable
#      target does not have the ``WIN32_EXECUTABLE`` property set.
#    * The tool png2ico is required. See :find-module:`FindPng2Ico`.
#    * Supported sizes: 16, 32, 48, 64, 128.
#
# Mac OS X notes
#    * The executable target must have the ``MACOSX_BUNDLE`` property set.
#    * Icons are added to the bundle.
#    * The tool iconutil (provided by Apple) is required.
#    * Supported sizes: 16, 32, 64, 128, 256, 512, 1024.
#    * At least a 128x128px icon is required.
#    * Larger sizes are automatically used to substitute for smaller sizes on
#      "Retina" (high-resolution) displays. For example, a 32px icon, if
#      provided, will be used as a 32px icon on standard-resolution displays,
#      and as a 16px-equivalent icon (with an "@2x" tag) on high-resolution
#      displays.
#    * This function sets the ``MACOSX_BUNDLE_ICON_FILE`` variable to the name
#      of the generated icns file, so that it will be used as the
#      ``MACOSX_BUNDLE_ICON_FILE`` target property when you call
#      ``add_executable``.
#
# Since 1.7.0.

#=============================================================================
# Copyright 2014 Alex Merry <alex.merry@kde.org>
# Copyright 2014 Ralf Habacker <ralf.habacker@freenet.de>
# Copyright 2006-2009 Alexander Neundorf, <neundorf@kde.org>
# Copyright 2006, 2007, Laurent Montel, <montel@kde.org>
# Copyright 2007 Matthias Kretz <kretz@kde.org>
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

function(ecm_add_app_icon appsources)
    set(options)
    set(oneValueArgs)
    set(multiValueArgs ICONS)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT ARG_ICONS)
        message(FATAL_ERROR "No ICONS argument given to ecm_add_app_icon")
    endif()
    if(ARG_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "Unexpected arguments to ecm_add_app_icon: ${ARG_UNPARSED_ARGUMENTS}")
    endif()

    set(known_sizes 16 32 48 64 128 256 512 1024)
    foreach(size ${known_sizes})
        set(icons_at_${size}px)
    endforeach()

    foreach(icon ${ARG_ICONS})
        get_filename_component(icon_full ${icon} ABSOLUTE)
        if (NOT EXISTS "${icon_full}")
            message(AUTHOR_WARNING "${icon_full} does not exist, ignoring")
        else()
            get_filename_component(icon_name ${icon} NAME)
            string(REGEX MATCH "([0-9]+)\\-[^/]+\\.([a-z]+)$"
                               _dummy "${icon_name}")
            set(size  "${CMAKE_MATCH_1}")
            set(ext   "${CMAKE_MATCH_2}")
            if (NOT size)
                message(AUTHOR_WARNING "${icon_full} is not named correctly for ecm_add_app_icon - ignoring")
            elseif (NOT ext STREQUAL "png")
                message(AUTHOR_WARNING "${icon_full} is not a png file - ignoring")
            else()
                list(FIND known_sizes "${size}" offset)
                if (offset GREATER -1)
                    list(APPEND icons_at_${size}px "${icon_full}")
                endif()
            endif()
        endif()
    endforeach()

    set(mac_icons ${icons_at_16px}
                  ${icons_at_32px}
                  ${icons_at_64px}
                  ${icons_at_128px}
                  ${icons_at_256px}
                  ${icons_at_512px}
                  ${icons_at_1024px})
    if (NOT icons_at_128px)
        message(AUTHOR_WARNING "No 128px icon provided; this will not work on Mac OS X")
    endif()

    set(windows_icons ${icons_at_16px}
                      ${icons_at_32px}
                      ${icons_at_48px}
                      ${icons_at_64px}
                      ${icons_at_128px})
    if (NOT windows_icons)
        message(AUTHOR_WARNING "No icons suitable for use on Windows provided")
    endif()

    set (_outfilename "${CMAKE_CURRENT_BINARY_DIR}/${appsources}")

    if (WIN32 AND windows_icons)
        set(saved_CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}")
        set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_FIND_MODULE_DIR})
        find_package(Png2Ico)
        set(CMAKE_MODULE_PATH "${saved_CMAKE_MODULE_PATH}")

        if (Png2Ico_FOUND)
            if (Png2Ico_HAS_RCFILE_ARGUMENT)
                add_custom_command(
                    OUTPUT "${_outfilename}.rc" "${_outfilename}.ico"
                    COMMAND Png2Ico::Png2Ico
                    ARGS
                        --rcfile "${_outfilename}.rc"
                        "${_outfilename}.ico"
                        ${windows_icons}
                    DEPENDS ${windows_icons}
                    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
                )
            else()
                add_custom_command(
                    OUTPUT "${_outfilename}.ico"
                    COMMAND Png2Ico::Png2Ico
                    ARGS "${_outfilename}.ico" ${windows_icons}
                    DEPENDS ${windows_icons}
                    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
                )
                # this bit's a little hacky to make the dependency stuff work
                file(WRITE "${_outfilename}.rc.in" "IDI_ICON1        ICON        DISCARDABLE    \"${_outfilename}.ico\"\n")
                add_custom_command(
                    OUTPUT "${_outfilename}.rc"
                    COMMAND ${CMAKE_COMMAND}
                    ARGS -E copy "${_outfilename}.rc.in" "${_outfilename}.rc"
                    DEPENDS "${_outfilename}.ico"
                    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
                )
            endif()
            set(${appsources} "${${appsources}};${_outfilename}.rc" PARENT_SCOPE)
        else()
            message(WARNING "Unable to find the png2ico utility - application will not have an application icon!")
        endif()
    elseif (APPLE AND mac_icons)
        # first generate .iconset directory structure, then convert to .icns format using the Mac OS X "iconutil" utility,
        # to create retina compatible icon, you need png source files in pixel resolution 16x16, 32x32, 64x64, 128x128,
        # 256x256, 512x512, 1024x1024
        find_program(ICONUTIL_EXECUTABLE NAMES iconutil)
        if (ICONUTIL_EXECUTABLE)
            add_custom_command(
                OUTPUT "${_outfilename}.iconset"
                COMMAND ${CMAKE_COMMAND}
                ARGS -E make_directory "${_outfilename}.iconset"
            )
            set(iconset_icons)
            macro(copy_icon filename sizename)
                add_custom_command(
                    OUTPUT "${_outfilename}.iconset/icon_${sizename}.png"
                    COMMAND ${CMAKE_COMMAND}
                    ARGS -E copy
                         "${filename}"
                         "${_outfilename}.iconset/icon_${sizename}.png"
                    DEPENDS
                        "${_outfilename}.iconset"
                        "${filename}"
                    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
                )
                list(APPEND iconset_icons
                            "${_outfilename}.iconset/icon_${sizename}.png")
            endmacro()
            foreach(size 16 32 64 128 256 512)
                math(EXPR double_size "2 * ${size}")
                foreach(file ${icons_at_${size}px})
                    copy_icon("${file}" "${size}x${size}")
                endforeach()
                foreach(file ${icons_at_${double_size}px})
                    copy_icon("${file}" "${size}x${size}@2x")
                endforeach()
            endforeach()

            # generate .icns icon file
            add_custom_command(
                OUTPUT "${_outfilename}.icns"
                COMMAND ${ICONUTIL_EXECUTABLE}
                ARGS
                    --convert icns
                    --output "${_outfilename}.icns"
                    "${_outfilename}.iconset"
                DEPENDS ${iconset_icons}
                WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
            )
            # This will register the icon into the bundle
            set(MACOSX_BUNDLE_ICON_FILE ${appsources}.icns PARENT_SCOPE)

            # Append the icns file to the sources list so it will be a dependency to the
            # main target
            set(${appsources} "${${appsources}};${_outfilename}.icns" PARENT_SCOPE)

            # Install the icon into the Resources dir in the bundle
            set_source_files_properties(${_outfilename}.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
        else()
            message(STATUS "Unable to find the iconutil utility - application will not have an application icon!")
        endif()
    endif()
endfunction()
