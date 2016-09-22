find_package(LLVM REQUIRED CONFIG)
find_package(Boost REQUIRED system filesystem)

set(CHIG_PUBLIC_FILES
	include/chig/ChigModule.hpp
	include/chig/LangModule.hpp
	include/chig/NodeType.hpp
	include/chig/Context.hpp
	include/chig/ImportedModule.hpp
	include/chig/GraphFunction.hpp
	include/chig/NodeInstance.hpp
)

set(CHIG_PRIVATE_FILES
	src/Context.cpp
	src/ImportedModule.cpp
	src/NodeInstance.cpp
	src/GraphFunction.cpp
	src/LangModule.cpp
)

add_definitions("-D__STDC_LIMIT_MACROS")
add_definitions("-D__STDC_CONSTANT_MACROS")

add_library(chig SHARED ${CHIG_PUBLIC_FILES} ${CHIG_PRIVATE_FILES})

target_include_directories(chig
	PUBLIC
	${LLVM_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIR}
	include/
)

target_compile_features(chig PUBLIC cxx_constexpr cxx_rvalue_references cxx_variable_templates)

llvm_map_components_to_libnames(llvm_libs core irreader)


target_link_libraries(chig
	PUBLIC
	${llvm_libs} 
	${Boost_LIBRARIES}
)
