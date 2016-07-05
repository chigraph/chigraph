
add_executable(MetadataParse main.cpp)
target_link_libraries(MetadataParse chig)

# compile the ll into a bc
add_custom_target(bcgen
	COMMAND llvm-as test.ll -o "${CMAKE_CURRENT_BINARY_DIR}/test.bc"
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_dependencies(MetadataParse bcgen)

add_test(NAME MetadataParse 
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	COMMAND MetadataParse
)
