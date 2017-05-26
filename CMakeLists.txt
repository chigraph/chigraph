cmake_minimum_required(VERSION 3.0)

project(chigraph)

# Make sure Qt escapes it's things right
if (${CMAKE_VERSION_MAJOR}.${CMAKE_VERSION_MINOR} GREATER 3.1 OR ${CMAKE_VERSION_MAJOR}.${CMAKE_VERSION_MINOR} EQUAL 3.1)
	cmake_policy(SET CMP0053 NEW)
endif()

set(CG_THIRDPARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/)

option(CG_BUILD_TESTS "Should unit tests be built?" ON)
option(CG_BUILD_EXAMPLES "Should the examples be built?" OFF)
option(CG_BUILD_DEBUGGER "Should the debugger be built?" ON)
option(CG_USE_SYSTEM_LIBGIT2 "Should the system try to find libgit2 from the system instead of the packaged version" OFF)
option(CG_USE_SYSTEM_BOOST "Should the system try to find boost from the system instead of the packaged version" OFF)
option(CG_INSTALL_STANDARD_CLANG_HEADERS "Should the system install the lib/clang folder? Set this to on if you are installing to somewhere other than the clang install prerix." OFF)

# this is really important to keep install-like paths
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# make sure the rpath is good to go
list(APPEND CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

if(${CMAKE_VERSION_MAJOR}.${CMAKE_VERSION_MINOR} VERSION_LESS 3.1)
	include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/CheckCXX11Features.cmake)
	CXX11_CHECK_FEATURE("variadic_templates" 2555 HAS_CXX11_VARIADIC_TEMPLATES)
else(${CMAKE_VERSION_MAJOR}.${CMAKE_VERSION_MINOR} VERSION_GREATER 3.1 OR ${CMAKE_VERSION_MAJOR}.${CMAKE_VERSION_MINOR} VERSION_EQUAL 3.1)
	target_compile_features(chigraphgui PRIVATE cxx_variable_templates)
endif()

# we're building them ourselves, so don't auto-link in on windows
add_definitions("-DBOOST_ALL_NO_LIB")

if (CG_USE_SYSTEM_BOOST)
	find_package(Boost REQUIRED COMPONENTS filesystem system program_options)
else()
	add_subdirectory(third_party/boost)
	set(Boost_FILESYSTEM_LIBRARY boost_filesystem)
	set(Boost_SYSTEM_LIBRARY boost_system)
	set(Boost_PROGRAM_OPTIONS_LIBRARY boost_program_options)
endif()

# compile libgit2
if (CG_USE_SYSTEM_LIBGIT2)
	find_package(libgit2 REQUIRED)
else()
	add_subdirectory(third_party/libgit2)
	set(LIBGIT2_LIBRARIES git2)
	set(LIBGIT2_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/libgit2/include)
endif()

# find llvm-config. This dictates which clang etc is uses.
# Clang MUST be installed in the same directory.
find_program(LLVM_CONFIG llvm-config REQUIRED)
message(STATUS "llvm-config found at ${LLVM_CONFIG}")

execute_process(COMMAND ${LLVM_CONFIG} --version OUTPUT_VARIABLE LLVM_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
message(STATUS "LLVM version: ${LLVM_VERSION}")

# add libchig
add_subdirectory(chi-ctollvm)
add_subdirectory(libchigraph)

# add the debugger module
if (CG_BUILD_DEBUGGER)
	add_subdirectory(libchigraphdebugger)
endif()

add_subdirectory(chi)

# Unit tests...
if(CG_BUILD_TESTS)
	enable_testing()
	add_subdirectory(test)
endif()

# Examples
if (CG_BUILD_EXAMPLES)
	add_subdirectory(examples)
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

