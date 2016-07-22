
add_executable(JSONReader main.cpp)
target_link_libraries(JSONReader chig)

add_test(NAME JSONReader 
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/..
	COMMAND ${CMAKE_CURRENT_BINARY_DIR}/JSONReader
)
