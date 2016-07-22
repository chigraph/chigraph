# This builds random external files for the tests

# compile the ll into a bc
add_custom_target(assembleadd_test
	COMMAND llvm-as add_test.ll -o "${CMAKE_CURRENT_BINARY_DIR}/../add_test.bc"
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

# use configure_file so it create a file dependency
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/helloworld.chig ${CMAKE_CURRENT_BINARY_DIR}/../helloworld.chig COPYONLY)
