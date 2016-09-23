
add_executable(MetadataParse main.cpp)
target_link_libraries(MetadataParse chig)


add_test(NAME MetadataParse 
	WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/../"
	COMMAND ${CMAKE_CURRENT_BINARY_DIR}/MetadataParse
)
