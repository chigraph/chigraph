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

# get system libraries to link to
execute_process(COMMAND ${LLVM_CONFIG} --system-libs OUTPUT_VARIABLE LLVM_SYSTEM_LIBS OUTPUT_STRIP_TRAILING_WHITESPACE)
string(REPLACE " " ";" LLVM_SYSTEM_LINK_LIST "${LLVM_SYSTEM_LIBS}")

# get preprocessor flags
execute_process(COMMAND ${LLVM_CONFIG} --cxxflags OUTPUT_VARIABLE LLVM_CXX_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
string(REPLACE " " ";" LLVM_CXX_FLAGS_LIST "${LLVM_CXX_FLAGS}")

# get ld flags
execute_process(COMMAND ${LLVM_CONFIG} --ldflags OUTPUT_VARIABLE LLVM_LD_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
string(REPLACE " " ";" LLVM_LD_FLAGS_LIST "${LLVM_LD_FLAGS}")

execute_process(COMMAND ${LLVM_CONFIG} --includedir OUTPUT_VARIABLE LLVM_INCLUDE_DIR  OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${LLVM_CONFIG} --libdir OUTPUT_VARIABLE CLANG_LIB_DIR  OUTPUT_STRIP_TRAILING_WHITESPACE)

add_executable(chi-ctollvm ctollvm.cpp)

target_include_directories(chi-ctollvm PRIVATE ${LLVM_INCLUDE_DIR})
target_compile_options(chi-ctollvm PRIVATE ${LLVM_CXX_FLAGS_LIST})

target_link_libraries(chi-ctollvm ${LLVM_LD_FLAGS_LIST})

set(CLANG_LIBS 
	clangRewrite
	clangFrontendTool
	clangCodeGen
	clangFrontend
	clangParse
	clangSerialization
	clangSema
	clangEdit
	clangAnalysis
	clangAST
	clangLex
	clangDriver
	clangBasic
)

# get clang libraries
message(STATUS "Looking for clang libs in ${CLANG_LIB_DIR}")
foreach(CLANG_LIB ${CLANG_LIBS})
	find_library(${CLANG_LIB}_LIBRARY ${CLANG_LIB} PATHS ${CLANG_LIB_DIR} NO_DEFAULT_PATH)
	
	if (NOT ${CLANG_LIB}_LIBRARY)
		message(FATAL_ERROR "Could not locate ${CLANG_LIB}: ${${CLANG_LIB}_LIBRARY}")
	endif()
	
	message(STATUS "${CLANG_LIB} found at ${${CLANG_LIB}_LIBRARY}")
	
	target_link_libraries(chi-ctollvm
		${${CLANG_LIB}_LIBRARY}
	)
	
endforeach()

target_link_libraries(chi-ctollvm ${LLVM_LINK_LIST} ${LLVM_SYSTEM_LINK_LIST} ${Boost_PROGRAM_OPTIONS_LIBRARY})

target_compile_features(chi-ctollvm PRIVATE cxx_variable_templates cxx_strong_enums)
if(${CMAKE_VERSION_MAJOR}.${CMAKE_VERSION_MINOR} LESS 3.1)
	CXX11_CHECK_FEATURE("constexpr"          2235 HAS_CXX11_CONSTEXPR)
else(${CMAKE_VERSION_MAJOR}.${CMAKE_VERSION_MINOR} GREATER 3.1 OR ${CMAKE_VERSION_MAJOR}.${CMAKE_VERSION_MINOR} EQUAL 3.1)
	target_compile_features(chi-ctollvm PRIVATE cxx_variable_templates cxx_strong_enums)
endif()


# move the clang lib folder, this is basic includes
add_custom_command(
TARGET chi-ctollvm POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_directory
	${CLANG_LIB_DIR}/clang
	${CMAKE_BINARY_DIR}/lib/clang
BYPRODUCTS ${CMAKE_BINARY_DIR}/lib/clang
COMMENT "Copying standard clang includes from ${CLANG_LIB_DIR}/clang to ${CMAKE_BINARY_DIR}/lib/clang"
)



