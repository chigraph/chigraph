
add_executable(JSONReader main.cpp)
target_link_libraries(JSONReader chig)

message(${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME JSONReader 
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	COMMAND ${CMAKE_CURRENT_BINARY_DIR}/JSONReader
)

