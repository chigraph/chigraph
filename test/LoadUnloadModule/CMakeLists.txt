
add_executable(LoadUnloadModule main.cpp)
target_link_libraries(LoadUnloadModule chig)

add_dependencies(LoadUnloadModule assembleadd_test)

add_test(NAME LoadUnloadModule
	WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/.."
	COMMAND ${CMAKE_CURRENT_BINARY_DIR}/LoadUnloadModule
)

