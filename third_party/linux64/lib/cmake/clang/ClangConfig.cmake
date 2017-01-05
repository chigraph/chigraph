# This file allows users to call find_package(Clang) and pick up our targets.

find_package(LLVM REQUIRED CONFIG)


# Compute the installation prefix from this LLVMConfig.cmake file location.
get_filename_component(CLANG_INSTALL_PREFIX "${CMAKE_CURRENT_LIST_FILE}" PATH)
get_filename_component(CLANG_INSTALL_PREFIX "${CLANG_INSTALL_PREFIX}" PATH)
get_filename_component(CLANG_INSTALL_PREFIX "${CLANG_INSTALL_PREFIX}" PATH)
get_filename_component(CLANG_INSTALL_PREFIX "${CLANG_INSTALL_PREFIX}" PATH)

set(CLANG_EXPORTED_TARGETS "clangBasic;clangLex;clangParse;clangAST;clangDynamicASTMatchers;clangASTMatchers;clangSema;clangCodeGen;clangAnalysis;clangEdit;clangRewrite;clangARCMigrate;clangDriver;clangSerialization;clangRewriteFrontend;clangFrontend;clangFrontendTool;clangToolingCore;clangTooling;clangIndex;clangStaticAnalyzerCore;clangStaticAnalyzerCheckers;clangStaticAnalyzerFrontend;clangFormat;libclang")
set(CLANG_CMAKE_DIR "${CLANG_INSTALL_PREFIX}/lib/cmake/clang")

# Provide all our library targets to users.
include("${CLANG_CMAKE_DIR}/ClangTargets.cmake")
