include_directories(../third_party/Catch/include)

set(TEST_SRCS
	main.cpp
	TestCommon.hpp
	ContextTests.cpp
	JSONSerializerTests.cpp
	LangModuleTests.cpp
	NameManglerTests.cpp
	GraphModuleTest.cpp
	GraphFunctionEntryTest.cpp
	GraphFunctionLocalsTest.cpp
	GraphFunctionExitTest.cpp
	GraphFunctionInOutsTest.cpp
	FetchTests.cpp
	SubprocessTest.cpp
)

set(DEBUGGER_TEST_SRCS
	DebuggerTests.cpp
)

if (CG_BUILD_DEBUGGER) 
	list(APPEND TEST_SRCS ${DEBUGGER_TEST_SRCS})
endif()

add_executable(api_tests ${TEST_SRCS})
target_link_libraries(api_tests PUBLIC chigraph)

if (CG_BUILD_DEBUGGER)
	target_link_libraries(api_tests PUBLIC chigraphdebugger)
endif()

add_test(NAME api_tests 
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	COMMAND api_tests
)

# add subprocess test executable
add_executable(subprocess_tester_child SubprocessTesterChild.cpp)
target_link_libraries(subprocess_tester_child)

add_dependencies(api_tests subprocess_tester_child)

add_subdirectory(error)
add_subdirectory(codegen)
