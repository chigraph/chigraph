cmake_minimum_required(VERSION 3.0)

project(chigraph)

# Make sure Qt escapes it's things right
if (${CMAKE_VERSION_MAJOR}.${CMAKE_VERSION_MINOR} GREATER 3.1 OR ${CMAKE_VERSION_MAJOR}.${CMAKE_VERSION_MINOR} EQUAL 3.1)
	cmake_policy(SET CMP0053 NEW)
endif()

set(CG_THIRDPARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/)

option(CG_BUILD_TESTS "Should unit tests be built?" ON)
option(CG_BUILD_NETWORK_TESTS "Should tests that require a network connection be ran?" ON)
option(CG_BUILD_EXAMPLES "Should the examples be built?" OFF)
option(CG_BUILD_DEBUGGER "Should the debugger be built?" ON)
option(CG_BUILD_FETCHER "Should the fetcher be built? Requires libgit2." ON)
option(CG_INSTALL_STANDARD_CLANG_HEADERS "Should the system install the lib/clang folder? Set this to on if you are installing to somewhere other than the clang install prefix." OFF)
option(CG_RUNTIME_DEUBG "Use a non-optimzied runtime and generate debug info for it" OFF)

# String options
set(CG_RUNTIME_CLANG_FLAGS "" CACHE STRING "Flags to pass to clang while generating runtime.bc")
set(CG_RUNTIME_COMPILER "" CACHE STRING "Compiler to compile the runtime with. If using llvm 3.5, this must be set to some newer clsng version.")

# this is really important to keep install-like paths
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# make sure the rpath is good to go
list(APPEND CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

# make sure this is a valid compiler
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	if ("${CMAKE_CXX_COMPILER_VERSION}" VERSION_LESS 5.0)
		message(FATAL_ERROR "GNU gcc version ${CMAKE_CXX_COMPILER_VERSION} is not new enough. Compiling chigraph requires gcc 5.0+ or clang 3.5+")
	endif()
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	if ("${CMAKE_CXX_COMPILER_VERSION}" VERSION_LESS 3.5)
		message(FATAL_ERROR "Clang ${CMAKE_CXX_COMPILER_VERSION} is not new enough. Compiling chigraph requires gcc 5.0+ or clang 3.5+")
	endif()
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
	if ("${CMAKE_CXX_COMPILER_VERSION}" VERSION_LESS 6.0)
		message(FATAL_ERROR "AppleClang ${CMAKE_CXX_COMPILER_VERSION} is not new enough. Compiling chigraph requires XCode 6.0+")
	endif()
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	if ("${CMAKE_CXX_COMPILER_VERSION}" VERSION_LESS 19.0)
		message(FATAL_ERROR "MSVC ${CMAKE_CXX_COMPILER_VERSION} is not new enough. Compiling chigraph requires Visual Studio 2015+")
	endif()
endif()

# find llvm-config. This dictates which clang etc is uses.
# Clang MUST be installed in the same directory.
find_program(LLVM_CONFIG llvm-config REQUIRED)
message(STATUS "llvm-config found at ${LLVM_CONFIG}")

execute_process(COMMAND ${LLVM_CONFIG} --version OUTPUT_VARIABLE LLVM_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
message(STATUS "LLVM version: ${LLVM_VERSION}")
if ("${LLVM_VERSION}" VERSION_LESS 3.5)
	message(FATAL_ERROR "LLVM version ${LLVM_VERSION} is not new enough. Compiling chigraph requires LLVM 3.5+")
endif()

# see if llvm is built with expensive checks, which requires us to add -D_GLIBCXX_DEBUG to boost
if (UNIX AND NOT APPLE)
	execute_process(COMMAND ${LLVM_CONFIG} --cppflags OUTPUT_VARIABLE LLVM_CPPFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
	
	if (LLVM_CPPFLAGS MATCHES ".*EXPENSIVE_CHECKS.*")
		add_definitions("-D_GLIBCXX_DEBUG")
		message(STATUS "Adding -D_GLIBCXX_DEBUG because LLVM was built with expensive checks")
	endif()
	
endif()

# we're building them ourselves, so don't auto-link in on windows
add_definitions("-DBOOST_ALL_NO_LIB")

find_package(Boost REQUIRED COMPONENTS program_options)

# compile libgit2 if required
if (CG_BUILD_FETCHER)
	find_package(libgit2 REQUIRED)
endif()


# add the chigraph libraries
add_subdirectory(lib)

add_subdirectory(chi)

# Unit tests...
if(CG_BUILD_TESTS)
	enable_testing()
	add_subdirectory(test)
endif()

# Examples
add_subdirectory(examples)


# Documentation

find_package(Doxygen)

if(DOXYGEN_FOUND AND DOXYGEN_DOT_FOUND)
	
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in ${CMAKE_BINARY_DIR}/Doxyfile)
	
	add_custom_target(doc
		COMMAND ${DOXYGEN_EXECUTABLE}
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
		COMMENT "Generating Doxygen Docs..."
		DEPENDS ${CHIGRAPH_PRIVATE_FILES} ${CHIGRAPH_PUBLIC_FILES}
	)
	
else()
	message(WARNING "either doxygen or dot not found, target `doc` will not be avaliable")
endif()

