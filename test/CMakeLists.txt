
# Setup catch
add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/Catch)

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
	SubprocessTest.cpp
	ResultTest.cpp
)

set(DEBUGGER_TEST_SRCS
	DebuggerTests.cpp
)
set(FETCH_TEST_SRCS
	FetchTests.cpp
)

if (CG_BUILD_FETCHER AND CG_BUILD_NETWORK_TESTS)
	list(APPEND TEST_SRCS ${FETCH_TEST_SRCS})
endif()

if (CG_BUILD_DEBUGGER) 
	list(APPEND TEST_SRCS ${DEBUGGER_TEST_SRCS})
endif()

add_executable(api_tests ${TEST_SRCS})
target_link_libraries(api_tests PUBLIC chigraphcore Catch)

set_property(TARGET api_tests PROPERTY CXX_STANDARD 17)
set_property(TARGET api_tests PROPERTY CXX_STANDARD_REQUIRED ON)


if (CG_BUILD_FETCHER)
	target_link_libraries(api_tests PUBLIC chigraphfetcher)
endif()
if (CG_BUILD_DEBUGGER)
	target_link_libraries(api_tests PUBLIC chigraphdebugger)
endif()

add_test(NAME api_tests 
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	COMMAND api_tests
)

# add subprocess test executable
add_executable(subprocess_tester_child SubprocessTesterChild.cpp)
	
# and make sure we get the compile option to enable it if the compiler needs it
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR 
	CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang" OR
	CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_compile_options(subprocess_tester_child PUBLIC -std=c++17)
endif()


add_dependencies(api_tests subprocess_tester_child)

add_subdirectory(error)
add_subdirectory(codegen)
