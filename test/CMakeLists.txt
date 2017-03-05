include_directories(../third_party/Catch/include)

# make llvm happy
add_definitions("-D__STDC_LIMIT_MACROS")
add_definitions("-D__STDC_CONSTANT_MACROS")

set(TEST_SRCS
	main.cpp
	TestCommon.hpp
	ContextTests.cpp
	JSONSerializer.cpp
	LangModuleTests.cpp
	NameManglerTests.cpp
	GraphModuleTest.cpp
	GraphFunctionEntryTest.cpp
	GraphFunctionLocalsTest.cpp
	GraphFunctionExitTest.cpp
	GraphFunctionInOutsTest.cpp
)

set(DEBUGGER_TEST_SRCS
	DebuggerTests.cpp
)

if (CG_BUILD_DEBUGGER) 
	list(APPEND TEST_SRCS ${DEBUGGER_TEST_SRCS})
endif()

add_executable(api_tests ${TEST_SRCS})
target_link_libraries(api_tests PUBLIC libchig)

if (CG_BUILD_DEBUGGER)
	target_link_libraries(api_tests PUBLIC chigdebugger)
endif()

add_test(NAME api_tests 
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	COMMAND api_tests
)

add_subdirectory(error)
add_subdirectory(codegen)
