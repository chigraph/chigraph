execute_process(COMMAND ${LLVM_CONFIG} --prefix OUTPUT_VARIABLE CLANG_INSTALL_PREFIX  OUTPUT_STRIP_TRAILING_WHITESPACE)
message(STATUS "clang install prefix: ${CLANG_INSTALL_PREFIX}")

execute_process(COMMAND ${LLVM_CONFIG} --libdir OUTPUT_VARIABLE CLANG_LIB_DIR  OUTPUT_STRIP_TRAILING_WHITESPACE)
message(STATUS "clang libdir: ${CLANG_LIB_DIR}")

find_program(LLI_EXE lli REQUIRED HINTS ${CLANG_INSTALL_PREFIX}/bin)
message(STATUS "lli: ${LLI_EXE}")

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

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.hpp.in ${CMAKE_BINARY_DIR}/chig/Config.hpp)

target_compile_features(libchig PUBLIC cxx_constexpr cxx_rvalue_references cxx_variable_templates)

# get include directories
execute_process(COMMAND ${LLVM_CONFIG} --includedir OUTPUT_VARIABLE LLVM_INCLUDE_DIR  OUTPUT_STRIP_TRAILING_WHITESPACE)
message(STATUS "LLVM include directory: ${LLVM_INCLUDE_DIR}")

# get llvm libraries
set(LLVM_COMPONENTS
	irreader
	support
	bitwriter
	executionengine
	native
	nativecodegen
	linker
	mcjit
	codegen
	selectiondag
)
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

target_link_libraries(libchig
PUBLIC
	libexecstream
	boost_filesystem
	${LLVM_SYSTEM_LINK_LIST}
	${LLVM_LINK_LIST}
	${LLVM_LD_FLAGS_LIST}
	ffi # llvm often forgets to put this in --system-libs, so just link it anyways just in case
PRIVATE
	ctollvm
)

target_include_directories(libchig
PUBLIC
	include/
	${CMAKE_BINARY_DIR}/ # for config.h
	../third_party/GSL/include
	../third_party/libexecstream/include
	${LLVM_INCLUDE_DIR}
)

target_compile_options(libchig
PUBLIC 
	${LLVM_CXX_FLAGS_LIST}
)


# move the clang lib folder, this is basic includes
add_custom_command(
TARGET libchig POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_directory
	${CLANG_LIB_DIR}/clang
	${CMAKE_BINARY_DIR}/lib/clang
BYPRODUCTS ${CMAKE_BINARY_DIR}/lib/clang
COMMENT "Copying standard clang includes from ${CLANG_LIB_DIR}/clang to ${CMAKE_BINARY_DIR}/lib/clang"
)

install(TARGETS libchig DESTINATION lib)
install(DIRECTORY include/chig DESTINATION include)

