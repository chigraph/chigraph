file(READ "${TARGET_DIR}/CMakeFiles/${TARGET_NAME}.dir/link.txt" out)

string(FIND "${out}" "-o ${TARGET_NAME}" POS) #we trim the initial arguments, we want the ones in the end. we find the target
string(SUBSTRING "${out}" ${POS} -1 out) #we
string(REGEX MATCHALL "(/|\\.\\./|\\./)[^ ]+\\.so" outout "${out}")
string(STRIP "${outout}" outout)
string(REPLACE " /" ";/" outout "${outout}")

set(extralibs)
foreach(lib IN LISTS outout) #now we filter Qt5 libraries, because Qt wants to take care about these itself
    if(NOT ${lib} MATCHES ".*/libQt5.*")
        # resolve relative paths
        if(${lib} MATCHES "^(\\.\\./|\\./)")
            set(lib "${TARGET_DIR}/${lib}")
        endif()
        if(extralibs)
            set(extralibs "${extralibs},${lib}")
        else()
            set(extralibs "${lib}")
        endif()
    endif()
endforeach()
if(extralibs)
    set(extralibs "\"android-extra-libs\": \"${extralibs}\",")
endif()

set(extraplugins)
foreach(folder "share" "lib/qml") #now we check for folders with extra stuff
    set(plugin "${EXPORT_DIR}/${folder}")
    if(EXISTS "${plugin}")
        if(extraplugins)
            set(extraplugins "${extraplugins},${plugin}")
        else()
            set(extraplugins "${plugin}")
        endif()
    endif()
endforeach()
if(extraplugins)
    set(extraplugins "\"android-extra-plugins\": \"${extraplugins}\",")
endif()

file(READ "${INPUT_FILE}" CONTENTS)
string(REPLACE "##EXTRALIBS##" "${extralibs}" NEWCONTENTS "${CONTENTS}")
string(REPLACE "##EXTRAPLUGINS##" "${extraplugins}" NEWCONTENTS "${NEWCONTENTS}")
file(WRITE "${OUTPUT_FILE}" ${NEWCONTENTS})
