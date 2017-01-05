#.rst:
# ECMMarkNonGuiExecutable
# -----------------------
#
# Marks an executable target as not being a GUI application.
#
# ::
#
#   ecm_mark_nongui_executable(<target1> [<target2> [...]])
#
# This will indicate to CMake that the specified targets should not be included
# in a MACOSX_BUNDLE and should not be WIN32_EXECUTABLEs.  On platforms other
# than MacOS X or Windows, this will have no effect.
#
# Since pre-1.0.0.

#=============================================================================
# Copyright 2012 Stephen Kelly <steveire@gmail.com>
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

function(ecm_mark_nongui_executable)
  foreach(_target ${ARGN})
    set_target_properties(${_target}
                            PROPERTIES
                            WIN32_EXECUTABLE FALSE
                            MACOSX_BUNDLE FALSE
                          )
  endforeach()
endfunction()
