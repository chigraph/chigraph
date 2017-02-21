
find_package(Clang REQUIRED)

add_library(ctollvm SHARED ctollvm.cpp)

target_include_directories(ctollvm PUBLIC ${CLANG_INCLUDE_DIRS})

target_compile_features(ctollvm PUBLIC cxx_constexpr cxx_rvalue_references cxx_variable_templates)

execute_process(COMMAND ${LLVM_CONFIG} --includedir OUTPUT_VARIABLE LLVM_INCLUDE_DIR  OUTPUT_STRIP_TRAILING_WHITESPACE)
message(STATUS "LLVM include directory: ${LLVM_INCLUDE_DIR}")
target_include_directories(ctollvm PUBLIC "${LLVM_INCLUDE_DIR}")

execute_process(COMMAND ${LLVM_CONFIG} --libs irreader support core bitwriter bitreader coverage transformutils ipo objcarcopts target option OUTPUT_VARIABLE LLVM_LIBRARIES OUTPUT_STRIP_TRAILING_WHITESPACE)
string(REPLACE " " ";" LLVM_LINK_LIST "${LLVM_LIBRARIES}")
message(STATUS "LLVM link libraries: ${LLVM_LINK_LIST}")
target_link_libraries(ctollvm PUBLIC ${LLVM_LINK_LIST})

execute_process(COMMAND ${LLVM_CONFIG} --system-libs OUTPUT_VARIABLE LLVM_SYSTEM_LIBS OUTPUT_STRIP_TRAILING_WHITESPACE)
string(REPLACE " " ";" LLVM_SYSTEM_LINK_LIST "${LLVM_SYSTEM_LIBS}")
message(STATUS "LLVM system libraries: ${LLVM_SYSTEM_LINK_LIST}")
target_link_libraries(ctollvm PUBLIC ${LLVM_SYSTEM_LINK_LIST})

target_link_libraries(ctollvm PUBLIC clangFrontend clangDriver clangSerialization clangParse
    clangCodeGen  clangSema clangAnalysis clangEdit clangAST clangLex
    clangBasic
  ${LLVMLIBS}
)
