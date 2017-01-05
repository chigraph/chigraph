#.rst:
# ECMMarkAsTest
# -------------
#
# Marks a target as only being required for tests.
#
# ::
#
#   ecm_mark_as_test(<target1> [<target2> [...]])
#
# This will cause the specified targets to not be built unless either
# BUILD_TESTING is set to ON or the user invokes the ``buildtests`` target.
#
# BUILD_TESTING is created as a cache variable by the CTest module and by the
# :kde-module:`KDECMakeSettings` module.
#
# Since pre-1.0.0.

#=============================================================================
# Copyright 2012 Stephen Kelly <steveire@gmail.com>
# Copyright 2012 Alex Neundorf <neundorf@kde.org>
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

if (NOT BUILD_TESTING)
  if(NOT TARGET buildtests)
    add_custom_target(buildtests)
  endif()
endif()

function(ecm_mark_as_test)
  if (NOT BUILD_TESTING)
    foreach(_target ${ARGN})
      set_target_properties(${_target}
                              PROPERTIES
                              EXCLUDE_FROM_ALL TRUE
                           )
      add_dependencies(buildtests ${_target})
    endforeach()
  endif()
endfunction()
