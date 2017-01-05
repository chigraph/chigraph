
# Copyright (c) 2006, Alexander Neundorf, <neundorf@kde.org>
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

EXECUTE_PROCESS(COMMAND ${KDE_UIC_EXECUTABLE}
  -tr tr2i18n
  ${KDE_UIC_FILE}
  OUTPUT_VARIABLE _uic_CONTENTS
)

set(KDE_UIC_CPP_FILE ${KDE_UIC_H_FILE})


IF (_uic_CONTENTS)
  #replace tr218n("") with QString::null to avoid warning from KLocale
  STRING(REGEX REPLACE "tr2i18n\\(\"\"\\)" "QString::null" _uic_CONTENTS "${_uic_CONTENTS}" )
  STRING(REGEX REPLACE "tr2i18n\\(\"\", \"\"\\)" "QString::null" _uic_CONTENTS "${_uic_CONTENTS}" )
  #replace image15_data with img15_filename to make enable_final work
  STRING(REGEX REPLACE "image([0-9]+)_data" "img\\1_${KDE_UIC_BASENAME}" _uic_CONTENTS "${_uic_CONTENTS}")
  #fixup include guard
  STRING(REGEX REPLACE "#ifndef " "#ifndef UI_" _uic_CONTENTS "${_uic_CONTENTS}")
  STRING(REGEX REPLACE "#define " "#define UI_" _uic_CONTENTS "${_uic_CONTENTS}")

  FILE(WRITE ${KDE_UIC_CPP_FILE} "#include <klocalizedstring.h>\n\n${_uic_CONTENTS}\n")
ENDIF()

