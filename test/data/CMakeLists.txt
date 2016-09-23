# This builds random external files for the tests

find_package(LLVM)


# use configure_file so it create a file dependency
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/helloworld.chig ${CMAKE_CURRENT_BINARY_DIR}/../helloworld.chig COPYONLY)
