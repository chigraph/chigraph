
find_program(LLI_EXE lli REQUIRED HINTS ${LLVM_INSTALL_PREFIX}/bin)
message(STATUS "lli:" ${LLI_EXE})

find_program(CLANG_EXE clang REQUIRED HINTS ${LLVM_INSTALL_PREFIX}/bin)
message(STATUS "clang: ${CLANG_EXE}")


get_filename_component(CLANG_BIN_DIR ${CLANG_EXE} DIRECTORY)
get_filename_component(CLANG_INSTALL_PREFIX ${CLANG_BIN_DIR} DIRECTORY)
message(STATUS "clang install prefix: ${CLANG_INSTALL_PREFIX}")

find_library(CLANG_BASIC_LIB clangBasic REQUIRED HINTS ${CLANG_INSTALL_PREFIX}/lib ${CLANG_INSTALL_PREFIX}/lib64)
get_filename_component(CLANG_LIB_DIR ${CLANG_BASIC_LIB} DIRECTORY)
message(STATUS "clang libdir: ${CLANG_LIB_DIR}")

add_subdirectory(ctollvm)

set(CHIG_PUBLIC_FILES
	include/chig/ChigModule.hpp
	include/chig/Context.hpp
	include/chig/GraphFunction.hpp
	include/chig/GraphModule.hpp
	include/chig/LangModule.hpp
	include/chig/NodeInstance.hpp
	include/chig/NodeType.hpp
	include/chig/Result.hpp
	include/chig/NameMangler.hpp
	include/chig/CModule.hpp
	include/chig/FunctionValidator.hpp
	include/chig/FunctionCompiler.hpp
	include/chig/JsonDeserializer.hpp
	include/chig/JsonSerializer.hpp
	include/chig/GraphStruct.hpp
)

set(CHIG_PRIVATE_FILES
	src/Context.cpp
	src/NodeInstance.cpp
	src/GraphFunction.cpp
	src/LangModule.cpp
	src/GraphModule.cpp
	src/ChigModule.cpp
	src/CModule.cpp
	src/Result.cpp
	src/NameMangler.cpp
	src/FunctionValidator.cpp
	src/FunctionCompiler.cpp
	src/JsonDeserializer.cpp
	src/JsonSerializer.cpp
	src/GraphStruct.cpp
)


add_library(libchig STATIC ${CHIG_PUBLIC_FILES} ${CHIG_PRIVATE_FILES})
set_target_properties(libchig PROPERTIES OUTPUT_NAME chig)

set(TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../test)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.hpp.in ${CMAKE_BINARY_DIR}/chig/Config.hpp)

target_include_directories(libchig
	PUBLIC
	${LLVM_INCLUDE_DIRS}
	include/
	${CMAKE_BINARY_DIR}/
	../third_party/GSL/include
	../third_party/libexecstream/include
)


target_compile_features(libchig PUBLIC cxx_constexpr cxx_rvalue_references cxx_variable_templates)

execute_process(COMMAND ${LLVM_CONFIG} --includedir OUTPUT_VARIABLE LLVM_INCLUDE_DIR  OUTPUT_STRIP_TRAILING_WHITESPACE)
message(STATUS "LLVM include directory: ${LLVM_INCLUDE_DIR}")
target_include_directories(libchig PUBLIC "${LLVM_INCLUDE_DIR}")

execute_process(COMMAND ${LLVM_CONFIG} --libs irreader support bitwriter executionengine native nativecodegen interpreter linker orcjit mcjit codegen selectiondag OUTPUT_VARIABLE LLVM_LIBRARIES OUTPUT_STRIP_TRAILING_WHITESPACE)
string(REPLACE " " ";" LLVM_LINK_LIST "${LLVM_LIBRARIES}")
message(STATUS "LLVM link libraries: ${LLVM_LINK_LIST}")
target_link_libraries(libchig PUBLIC ${LLVM_LINK_LIST})

execute_process(COMMAND ${LLVM_CONFIG} --system-libs OUTPUT_VARIABLE LLVM_SYSTEM_LIBS OUTPUT_STRIP_TRAILING_WHITESPACE)
string(REPLACE " " ";" LLVM_SYSTEM_LINK_LIST "${LLVM_SYSTEM_LIBS}")
message(STATUS "LLVM system libraries: ${LLVM_SYSTEM_LINK_LIST}")
target_link_libraries(libchig PUBLIC ${LLVM_SYSTEM_LINK_LIST})

execute_process(COMMAND ${LLVM_CONFIG} --cppflags OUTPUT_VARIABLE LLVM_CXX_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
string(REPLACE " " ";" LLVM_CXX_FLAGS_LIST "${LLVM_CXX_FLAGS}")
message(STATUS "LLVM cxx flags: ${LLVM_CXX_FLAGS_LIST}")
target_compile_options(libchig PUBLIC ${LLVM_CXX_FLAGS_LIST})



target_link_libraries(libchig PUBLIC ffi)

target_compile_definitions(libchig PUBLIC 
  -D_GNU_SOURCE
  -D__STDC_CONSTANT_MACROS
  -D__STDC_FORMAT_MACROS
  -D__STDC_LIMIT_MACROS
)

target_link_libraries(libchig
PUBLIC
	libexecstream
	boost_filesystem
PRIVATE
    ctollvm
)

# move the clang lib folder, this is basic includes
add_custom_command(
TARGET libchig POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_directory
	${CLANG_LIBDIR}/clang
	${CMAKE_BINARY_DIR}/lib/clang
)

install(TARGETS libchig DESTINATION lib)
install(DIRECTORY include/chig DESTINATION include)

