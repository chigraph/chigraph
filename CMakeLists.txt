cmake_minimum_required(VERSION 3.0)

project(chigraph)

cmake_policy(SET CMP0053 NEW)

set(CG_THIRDPARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/)

option(CG_BUILD_TESTS "Should unit tests be built?" ON)
option(CG_BUILD_GUI "Should the GUI be built" ON)
option(CG_BUILD_DEBUGGER "Should the debugger be built" ON)
option(CG_USE_SYSTEM_LIBGIT2 "Should the system try to find libgit2 from the system instead of the packaged version" OFF)

# this is really important to keep install-like paths
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# make sure the rpath is good to go
list(APPEND CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

# disable tests
set(BUILD_CLAR OFF CACHE BOOL "")
add_subdirectory(third_party/libexecstream)

# we're building them ourselves, so don't auto-link in on windows
add_definitions("-DBOOST_ALL_NO_LIB")
add_subdirectory(third_party/boost)

# compile libgit2
if (NOT CG_USE_SYSTEM_LIBGIT2)
	add_subdirectory(third_party/libgit2)
	set(LIBGIT2_LIBRARIES git2)
	set(LIBGIT2_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/libgit2/include)
else()
	find_package(libgit2 REQUIRED)
endif()

# find llvm-config. This dictates which clang etc is uses. 
# Clang MUST be installed in the same directory.
find_program(LLVM_CONFIG llvm-config REQUIRED)
message(STATUS "llvm-config found at ${LLVM_CONFIG}")

execute_process(COMMAND ${LLVM_CONFIG} --version OUTPUT_VARIABLE LLVM_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
message(STATUS "LLVM version: ${LLVM_VERSION}")

# add libchig
add_subdirectory(libchigraph)

# add the debugger module
if (CG_BUILD_DEBUGGER)
	add_subdirectory(libchigraphdebugger)
endif()

add_subdirectory(chi)

if(CG_BUILD_GUI)
	set(BUILD_EXAMPLES OFF CACHE BOOL "")
	add_subdirectory(third_party/nodeeditor)
	add_subdirectory(chigraphgui)
endif()

# Unit tests...
if(CG_BUILD_TESTS)
	enable_testing()
	add_subdirectory(test)
endif()

# Documentation

find_package(Doxygen)

if(DOXYGEN_FOUND)
	
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in ${CMAKE_BINARY_DIR}/Doxyfile)
	
	add_custom_target(doc
		COMMAND ${DOXYGEN_EXECUTABLE}
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
		COMMENT "Generating Doxygen Docs..."
		DEPENDS ${CHIGRAPH_PRIVATE_FILES} ${CHIGRAPH_PUBLIC_FILES}
	)
	
else()
	message(WARNING "DOXYGEN not found, target `doc` will not be avaliable")
endif()

