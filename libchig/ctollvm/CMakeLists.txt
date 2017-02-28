add_library(ctollvm SHARED ctollvm.cpp)

# require c++11
target_compile_features(ctollvm PUBLIC cxx_constexpr cxx_rvalue_references cxx_variable_templates)

execute_process(COMMAND ${LLVM_CONFIG} --includedir OUTPUT_VARIABLE LLVM_INCLUDE_DIR  OUTPUT_STRIP_TRAILING_WHITESPACE)
message(STATUS "LLVM include directory: ${LLVM_INCLUDE_DIR}")

execute_process(COMMAND ${LLVM_CONFIG} --libs irreader support core bitwriter bitreader transformutils ipo objcarcopts target option OUTPUT_VARIABLE LLVM_LIBRARIES OUTPUT_STRIP_TRAILING_WHITESPACE)
string(REPLACE " " ";" LLVM_LINK_LIST "${LLVM_LIBRARIES}")
message(STATUS "LLVM link libraries: ${LLVM_LINK_LIST}")

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

target_compile_options(ctollvm
PUBLIC
	${LLVM_CXX_FLAGS_LIST}
)

target_include_directories(ctollvm
PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${LLVM_INCLUDE_DIR}
)

target_link_libraries(ctollvm 
PRIVATE
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
PUBLIC
	${LLVM_LD_FLAGS_LIST}
	${LLVM_LINK_LIST}
	${LLVM_SYSTEM_LINK_LIST}
)
