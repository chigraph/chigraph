cmake_minimum_required(VERSION 2.8)

project(chigraph)

# add the module dirs
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/third_party/kf5)

set(CG_THIRDPARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/)

option(CG_BUILD_TESTS "Should unit tests be built?" ON)
option(CG_BUILD_GUI "Should the GUI be built" ON)

# disable tests
set(BUILD_CLAR OFF CACHE BOOL "")
add_subdirectory(third_party/tiny-process-library)
add_subdirectory(third_party/boost)


find_program(LLVM_CONFIG llvm-config)
message(STATUS "llvm-config found at ${LLVM_CONFIG}")

execute_process(COMMAND ${LLVM_CONFIG} --libdir OUTPUT_VARIABLE LLVM_LIBDIR OUTPUT_STRIP_TRAILING_WHITESPACE)
message(STATUS "LLVM library directory: ${LLVM_LIBDIR}")
link_directories(${LLVM_LIBDIR})

add_subdirectory(libchig)
add_subdirectory(chig)

if(CG_BUILD_GUI)
	set(BUILD_EXAMPLES OFF)
	add_subdirectory(third_party/nodeeditor)
	add_subdirectory(gui)
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
	message("DOXYGEN not found, target `make doc` will not be avaliable")
endif()

