add_library(ctollvm SHARED ctollvm.cpp)

# require c++11
target_compile_features(ctollvm PUBLIC cxx_constexpr cxx_rvalue_references cxx_variable_templates)

message(STATUS "Configuring ctollvm...")

execute_process(COMMAND ${LLVM_CONFIG} --includedir OUTPUT_VARIABLE LLVM_INCLUDE_DIR  OUTPUT_STRIP_TRAILING_WHITESPACE)
message(STATUS "LLVM include directory: ${LLVM_INCLUDE_DIR}")


execute_process(COMMAND ${LLVM_CONFIG} --system-libs OUTPUT_VARIABLE LLVM_SYSTEM_LIBS OUTPUT_STRIP_TRAILING_WHITESPACE)
string(REPLACE " " ";" LLVM_SYSTEM_LINK_LIST "${LLVM_SYSTEM_LIBS}")
message(STATUS "LLVM system libraries: ${LLVM_SYSTEM_LINK_LIST}")

# get preprocessor flags
execute_process(COMMAND ${LLVM_CONFIG} --cppflags OUTPUT_VARIABLE LLVM_CXX_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
string(REPLACE " " ";" LLVM_CXX_FLAGS_LIST "${LLVM_CXX_FLAGS}")
message(STATUS "LLVM cxx flags: ${LLVM_CXX_FLAGS_LIST}")

execute_process(COMMAND ${LLVM_CONFIG} --ldflags OUTPUT_VARIABLE LLVM_LD_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
string(REPLACE " " ";" LLVM_LD_FLAGS_LIST "${LLVM_LD_FLAGS}")
message(STATUS "LLVM ld flags: ${LLVM_LD_FLAGS_LIST}")

execute_process(COMMAND ${LLVM_CONFIG} --libdir OUTPUT_VARIABLE CLANG_LIB_DIR  OUTPUT_STRIP_TRAILING_WHITESPACE)

target_compile_options(ctollvm
PUBLIC
	${LLVM_CXX_FLAGS_LIST}
)

target_include_directories(ctollvm
PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${LLVM_INCLUDE_DIR}
)

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
	find_library(${CLANG_LIB}_LIBRARY ${CLANG_LIB} PATHS ${CLANG_LIB_DIR})
	
	if (NOT ${CLANG_LIB}_LIBRARY)
		message(FATAL_ERROR "Could not locate ${CLANG_LIB}: ${${CLANG_LIB}_LIBRARY}")
	endif()
	
	message(STATUS "${CLANG_LIB} found at ${${CLANG_LIB}_LIBRARY}")
	
	target_link_libraries(ctollvm 
	PRIVATE
		${${CLANG_LIB}_LIBRARY}
	)
	
endforeach()

target_link_libraries(ctollvm
PRIVATE
	${LLVM_LD_FLAGS_LIST}
	${LLVM_SYSTEM_LINK_LIST}
)

if(WIN32)

	find_library(VERSION_LIBRARY version)
	if (NOT VERSION_LIBRARY)
		message(FATAL_ERROR "Could not find version library")
	endif()
	message(STATUS "Linking to version library: ${VERSION_LIBRARY}")
	target_link_libraries(ctollvm
	PUBLIC
		${VERSION_LIBRARY}
	)
endif()

