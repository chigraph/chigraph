
add_executable(LoadUnloadModule main.cpp)
target_link_libraries(LoadUnloadModule chig)

add_dependencies(LoadUnloadModule assembleadd_test)

configure_file(${CMAKE_CURRENT_BINARY_DIR}/../add_test.bc ${CMAKE_CURRENT_BINARY_DIR}/../testdir/add_tester.bc COPYONLY)


add_test(NAME LoadUnloadModule
	WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/.."
	COMMAND ${CMAKE_CURRENT_BINARY_DIR}/LoadUnloadModule
)

