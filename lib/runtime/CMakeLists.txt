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

foreach(src ${RUNTIME_SRCS})
	set(output "${CMAKE_BINARY_DIR}/lib/runtime/runtime.obj/${src}.bc")
	add_custom_command(
		OUTPUT ${output}
		COMMAND ${CLANG_EXE} ${CMAKE_CURRENT_SOURCE_DIR}/${src} -std=gnu11 -c -emit-llvm -O3 -o ${output}
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
