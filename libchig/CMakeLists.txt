find_package(LLVM REQUIRED CONFIG)

find_program(CLANG_EXE clang)
find_program(LLI_EXE lli)

set(CHIG_PUBLIC_FILES
	include/chig/ChigModule.hpp
	include/chig/Context.hpp
	include/chig/GraphFunction.hpp
	include/chig/JsonModule.hpp
	include/chig/LangModule.hpp
	include/chig/NodeInstance.hpp
	include/chig/NodeType.hpp
	include/chig/Result.hpp
	include/chig/Graph.hpp
	include/chig/NameMangler.hpp
)

set(CHIG_PRIVATE_FILES
	src/Context.cpp
	src/NodeInstance.cpp
	src/GraphFunction.cpp
	src/LangModule.cpp
	src/JsonModule.cpp
	src/ChigModule.cpp
	src/CModule.cpp
	src/Graph.cpp
	src/NameMangler.cpp
)

add_definitions("-D__STDC_LIMIT_MACROS")
add_definitions("-D__STDC_CONSTANT_MACROS")

add_library(chigcore ${CHIG_PUBLIC_FILES} ${CHIG_PRIVATE_FILES})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.hpp.in ${CMAKE_BINARY_DIR}/chig/Config.hpp)

target_include_directories(chigcore
	PUBLIC
	${LLVM_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIR}
	include/
	${CMAKE_BINARY_DIR}/
	../third_party/GSL
)

target_compile_features(chigcore PUBLIC cxx_constexpr cxx_rvalue_references cxx_variable_templates)

llvm_map_components_to_libnames(llvm_libs core irreader support bitwriter executionengine native nativecodegen interpreter linker orcjit mcjit codegen )


target_link_libraries(chigcore
	PUBLIC
	${llvm_libs} 
	execstream
)

target_link_libraries(chigcore
PUBLIC
	debug ${Boost_SYSTEM_LIBRARY_DEBUG}
	optimized ${Boost_SYSTEM_LIBRARY_RELEASE}
	debug ${Boost_FILESYSTEM_LIBRARY_DEBUG}
	optimized ${Boost_FILESYSTEM_LIBRARY_RELEASE}
)

cotire(chigcore) # this speeds up builds (PCH, etc)

install(TARGETS chigcore DESTINATION lib)
install(DIRECTORY include/chig DESTINATION include)

