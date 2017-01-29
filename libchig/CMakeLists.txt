

find_program(CLANG_EXE clang)

find_program(LLI_EXE lli)

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


add_library(chigcore ${CHIG_PUBLIC_FILES} ${CHIG_PRIVATE_FILES})

set(TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../test)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.hpp.in ${CMAKE_BINARY_DIR}/chig/Config.hpp)

target_include_directories(chigcore
	PUBLIC
	${LLVM_INCLUDE_DIRS}
	include/
	${CMAKE_BINARY_DIR}/
	../third_party/GSL
	../third_party/tiny-process-library
)

target_compile_features(chigcore PUBLIC cxx_constexpr cxx_rvalue_references cxx_variable_templates)

execute_process(COMMAND ${LLVM_CONFIG} --includedir OUTPUT_VARIABLE LLVM_INCLUDE_DIR  OUTPUT_STRIP_TRAILING_WHITESPACE)
message(STATUS "LLVM include directory: ${LLVM_INCLUDE_DIR}")
target_include_directories(chigcore PUBLIC "${LLVM_INCLUDE_DIR}")

execute_process(COMMAND ${LLVM_CONFIG} --libs irreader support bitwriter executionengine native nativecodegen interpreter linker orcjit mcjit codegen selectiondag OUTPUT_VARIABLE LLVM_LIBRARIES OUTPUT_STRIP_TRAILING_WHITESPACE)
message(STATUS "LLVM link libraries: ${LLVM_LIBRARIES}")
target_link_libraries(chigcore PUBLIC ${LLVM_LIBRARIES})

execute_process(COMMAND ${LLVM_CONFIG} --system-libs OUTPUT_VARIABLE LLVM_SYSTEM_LIBS OUTPUT_STRIP_TRAILING_WHITESPACE)
message(STATUS "LLVM system libraries: ${LLVM_SYSTEM_LIBS}")
target_link_libraries(chigcore PUBLIC ${LLVM_SYSTEM_LIBS} ffi)

target_compile_definitions(chigcore PUBLIC 
  -D_GNU_SOURCE 
  -D__STDC_CONSTANT_MACROS 
  -D__STDC_FORMAT_MACROS 
  -D__STDC_LIMIT_MACROS
)

target_link_libraries(chigcore
PUBLIC
	tiny-process-library
	boost_filesystem
)

install(TARGETS chigcore DESTINATION lib)
install(DIRECTORY include/chig DESTINATION include)

