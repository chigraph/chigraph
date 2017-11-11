# chigraph runtime

execute_process(COMMAND ${LLVM_CONFIG} --version OUTPUT_VARIABLE LLVM_VER OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${LLVM_CONFIG} --bindir OUTPUT_VARIABLE LLVM_BINDIR OUTPUT_STRIP_TRAILING_WHITESPACE)

# get the short version of the version (ie 4.0 and not 4.0.1)
string(SUBSTRING ${LLVM_VER} 0 3 LLVM_VER)

# find clang to compile the C
find_program(CLANG_EXE
	NAMES clang-${LLVM_VER} clang
	PATHS ${LLVM_BINDIR}
	NO_DEFAULT_PATH
)
if (NOT EXISTS "${CLANG_EXE}")
	message(FATAL_ERROR "Failed to find clang exectuable in ${LLVM_BINDIR}/clang-${LLVM_VER} It is required to compile the runtime")
endif()
message(STATUS "Clang found at: ${CLANG_EXE}")

if("CG_RUNTIME_COMPILER" STREQUAL "")
    if ("LLVM_VERSION" STREQUAL 3.5)
        message(FATAL_ERROR "For llvm 3.5, a runtime compiler must be specified or else the runtimr will not compile")
	
    endif()
    set(CG_RUNTIME_COMPILER "${CLANG_EXE}")
endif()

# find llvm-link to link it into one .bc file
find_program(LLVM_LINK_EXE
	NAMES llvm-link-${LLVM_VER} llvm-link
	PATHS ${LLVM_BINDIR}
)
if (NOT EXISTS "${LLVM_LINK_EXE}")
	message(FATAL_ERROR "Failed to find llvm-link exectuable in ${LLVM_BINDIR} It is required to compile the runtime")
endif()
message(STATUS "llvm-link found at: ${LLVM_LINK_EXE}")

set(RUNTIME_SRCS
	main.c
	arc.c
)

# Create the dir for it
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/lib/chigraph)

set(RUNTIME_COMPILEDBC "")

# Generate the flags we need to pass to clang
set(clangflags "-std=gnu11")
if (CG_RUNTIME_DEBUG)
	list(APPEND clangflags "-O0" "-g")
else()
	list(APPEND clangflags "-O3")
endif()

# add these last so they have priority
list(APPEND clangflags ${CG_RUNTIME_CLANG_FLAGS})

foreach(src ${RUNTIME_SRCS})
	set(output "${CMAKE_BINARY_DIR}/lib/runtime/runtime.obj/${src}.bc")
	add_custom_command(
		OUTPUT ${output}
		COMMAND ${CG_RUNTIME_COMPILER} ${CMAKE_CURRENT_SOURCE_DIR}/${src} ${clangflags} -c -emit-llvm -o ${output}
		DEPENDS ${src}
		COMMENT "Generating ${src}.bc for runtime..."
	)
	list(APPEND RUNTIME_COMPILEDBC ${output})
endforeach()

add_custom_command(
	OUTPUT ${CMAKE_BINARY_DIR}/lib/chigraph/runtime.bc
	COMMAND ${LLVM_LINK_EXE} ${RUNTIME_COMPILEDBC} -o ${CMAKE_BINARY_DIR}/lib/chigraph/runtime.bc
	DEPENDS ${RUNTIME_COMPILEDBC}
	COMMENT "Generating runtime.bc..."
)

add_custom_target(
	chigraphruntime ALL
	DEPENDS ${CMAKE_BINARY_DIR}/lib/chigraph/runtime.bc
)

install(
	FILES ${CMAKE_BINARY_DIR}/lib/chigraph/runtime.bc 
	DESTINATION lib/chigraph/
)
