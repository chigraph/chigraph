# chigraph runtime

execute_process(COMMAND ${LLVM_CONFIG} --bindir OUTPUT_VARIABLE LLVM_BINDIR OUTPUT_STRIP_TRAILING_WHITESPACE)

set(CLANG_EXE "${LLVM_BINDIR}/clang${CMAKE_EXECUTABLE_SUFFIX}")
message(STATUS "Clang found at: ${CLANG_EXE}")

if (NOT EXISTS "${CLANG_EXE}")
	message(FATAL_ERROR "Failed to find clang exectuable. It is required to compile the runtime")
endif()

set(RUNTIME_SRCS
	main.c
)


# Create the dir for it
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/lib/chigraph)

add_custom_command(
	OUTPUT ${CMAKE_BINARY_DIR}/lib/chigraph/runtime.bc
	COMMAND ${CLANG_EXE} ${RUNTIME_SRCS} -c -emit-llvm -O3 -o ${CMAKE_BINARY_DIR}/lib/chigraph/runtime.bc
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	DEPENDS ${RUNTIME_SRCS}
	COMMENT "Generating runtime bitcode..."
)

add_custom_target(
	chigraphruntime ALL
	DEPENDS ${CMAKE_BINARY_DIR}/lib/chigraph/runtime.bc
)

install(
	FILES ${CMAKE_BINARY_DIR}/lib/chigraph/runtime.bc 
	DESTINATION lib/chigraph/
)
