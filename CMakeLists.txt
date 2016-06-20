cmake_minimum_required(VERSION 3.0)

project(chigraph)


option(CG_BUILD_TESTS "Should unit tests be built?" ON)

# Use our extra modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

add_subdirectory(chig)

# Unit tests...
if(CG_BUILD_TESTS)
	add_subdirectory(test)
endif()

# Documentation

find_package(Doxygen)

if(DOXYGEN_FOUND)
	
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_BINARY_DIR}/Doxyfile)
	
	add_custom_target(doc
		COMMAND ${DOXYGEN_EXECUTABLE}
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
		COMMENT "Generating Doxygen Docs..."
		DEPENDS ${CHIGRAPH_PRIVATE_FILES} ${CHIGRAPH_PUBLIC_FILES}
	)
	
else()
	message("DOXYGEN not found, target `make doc` will not be avaliable")
endif()

