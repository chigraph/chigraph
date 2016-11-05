include_directories(../third_party/Catch/include)

# make llvm happy
add_definitions("-D__STDC_LIMIT_MACROS")
add_definitions("-D__STDC_CONSTANT_MACROS")

add_subdirectory(data)


add_executable(tests main.cpp  Context.cpp)
target_link_libraries(tests PUBLIC chig)

add_test(NAME tests 
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	COMMAND ${CMAKE_CURRENT_BINARY_DIR}/tests
)
