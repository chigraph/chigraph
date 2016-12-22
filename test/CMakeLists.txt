include_directories(../third_party/Catch/include)

# make llvm happy
add_definitions("-D__STDC_LIMIT_MACROS")
add_definitions("-D__STDC_CONSTANT_MACROS")

add_executable(api_tests main.cpp ContextTests.cpp JSONSerializer.cpp LangModuleTests.cpp NameManglerTests.cpp)
target_link_libraries(api_tests PUBLIC chigcore)

add_test(NAME api_tests 
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	COMMAND api_tests
)

add_subdirectory(error)
add_subdirectory(codegen)
