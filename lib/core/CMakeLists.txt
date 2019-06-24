# chigraph CMakeLists.txt

# Create the library
set(CHI_PUBLIC_FILES
	include/chi/BitcodeParser.hpp
	include/chi/CCompiler.hpp
	include/chi/ChiModule.hpp
	include/chi/ClangFinder.hpp
	include/chi/Context.hpp
	include/chi/DataType.hpp
	include/chi/DefaultModuleCache.hpp
	include/chi/Dwarf.hpp
	include/chi/FunctionCompiler.hpp
	include/chi/FunctionValidator.hpp
	include/chi/Fwd.hpp
	include/chi/GraphFunction.hpp
	include/chi/GraphModule.hpp
	include/chi/GraphStruct.hpp
	include/chi/JsonDeserializer.hpp
	include/chi/JsonSerializer.hpp
	include/chi/LangModule.hpp
	include/chi/ModuleCache.hpp
	include/chi/NameMangler.hpp
	include/chi/NodeCompiler.hpp
	include/chi/NodeInstance.hpp
	include/chi/NodeType.hpp
	include/chi/Owned.hpp
)
set(CHI_PRIVATE_FILES
	src/BitcodeParser.cpp
	src/CCompiler.cpp
	src/ChiModule.cpp
	src/ClangFinder.cpp
	src/Context.cpp
	src/DataType.cpp
	src/DefaultModuleCache.cpp
	src/FunctionCompiler.cpp
	src/FunctionValidator.cpp
	src/GraphFunction.cpp
	src/GraphModule.cpp
	src/GraphStruct.cpp
	src/JsonDeserializer.cpp
	src/JsonSerializer.cpp
	src/LangModule.cpp
	src/NameMangler.cpp
	src/NodeCompiler.cpp
	src/NodeInstance.cpp
	src/NodeType.cpp
)
add_library(chigraphcore STATIC ${CHI_PUBLIC_FILES} ${CHI_PRIVATE_FILES})

# get llvm libraries
set(LLVM_COMPONENTS
	executionengine
	profiledata
	instrumentation
	irreader
	option
	bitwriter
	native
	interpreter
	lto
	objcarcopts
	nativecodegen
	linker
	mcjit
	scalaropts
	transformutils
	codegen
	selectiondag
)
if (LLVM_VERSION VERSION_GREATER 3.7.0 OR LLVM_VERSION VERSION_EQUAL 3.7.0)
	list(APPEND LLVM_COMPONENTS passes)
endif()
if (LLVM_VERSION VERSION_GREATER 3.9.0 OR LLVM_VERSION VERSION_EQUAL 3.9.0)
	list(APPEND LLVM_COMPONENTS coverage)
endif()
if (LLVM_VERSION VERSION_GREATER 4.0.0 OR LLVM_VERSION VERSION_EQUAL 4.0.0)
	list(APPEND LLVM_COMPONENTS coroutines)
endif()


execute_process(COMMAND ${LLVM_CONFIG} --libs ${LLVM_COMPONENTS} OUTPUT_VARIABLE LLVM_LIBRARIES OUTPUT_STRIP_TRAILING_WHITESPACE)
string(REPLACE " " ";" LLVM_LINK_LIST "${LLVM_LIBRARIES}")
message(STATUS "LLVM link libraries: ${LLVM_LINK_LIST}")

# get system libraries to link to
execute_process(COMMAND ${LLVM_CONFIG} --system-libs OUTPUT_VARIABLE LLVM_SYSTEM_LIBS OUTPUT_STRIP_TRAILING_WHITESPACE)
string(REPLACE " " ";" LLVM_SYSTEM_LINK_LIST "${LLVM_SYSTEM_LIBS}")
message(STATUS "LLVM system libraries: ${LLVM_SYSTEM_LINK_LIST}")

# get preprocessor flags
execute_process(COMMAND ${LLVM_CONFIG} --cppflags OUTPUT_VARIABLE LLVM_CXX_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
string(REPLACE " " ";" LLVM_CXX_FLAGS_LIST "${LLVM_CXX_FLAGS}")
message(STATUS "LLVM cxx flags: ${LLVM_CXX_FLAGS_LIST}")

# get ld flags
execute_process(COMMAND ${LLVM_CONFIG} --ldflags OUTPUT_VARIABLE LLVM_LD_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
string(REPLACE " " ";" LLVM_LD_FLAGS_LIST "${LLVM_LD_FLAGS}")
message(STATUS "LLVM ld flags: ${LLVM_LD_FLAGS_LIST}")

target_link_libraries(chigraphcore
PUBLIC
	${LLVM_LD_FLAGS_LIST}
	${LLVM_LINK_LIST}
	${LLVM_SYSTEM_LINK_LIST}
	chigraphsupport
)

# link to threads
find_package(Threads)
target_link_libraries(chigraphcore PRIVATE ${CMAKE_THREAD_LIBS_INIT})

# link ffi on unix
if (NOT WIN32)
	target_link_libraries(chigraphcore
	PUBLIC
		ffi # llvm often forgets to put this in --system-libs, so just link it anyways just in case
	)
endif()

# link to version
if(WIN32)

	target_link_libraries(chigraphcore
    PUBLIC
        version.lib
    )
endif()

# get include directories
execute_process(COMMAND ${LLVM_CONFIG} --includedir OUTPUT_VARIABLE LLVM_INCLUDE_DIR  OUTPUT_STRIP_TRAILING_WHITESPACE)

target_include_directories(chigraphcore
PUBLIC
	include/
	${LLVM_INCLUDE_DIR}
)

target_compile_options(chigraphcore
PUBLIC
	${LLVM_CXX_FLAGS_LIST}
)

# configure the config file
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/ChigraphConfig.cmake.in ${CMAKE_BINARY_DIR}/ChigraphConfig.cmake @ONLY)

# make sure runtime is built first
add_dependencies(chigraphcore chigraphruntime)

install(FILES ${CMAKE_BINARY_DIR}/ChigraphConfig.cmake DESTINATION lib/cmake/chigraph)

install(TARGETS chigraphcore
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)

install(DIRECTORY include/chi DESTINATION include)

