find_package(LLVM REQUIRED CONFIG)

set(CHIG_PUBLIC_FILES
	include/chig/ChigModule.hpp
	include/chig/Context.hpp
	include/chig/GraphFunction.hpp
	include/chig/JsonModule.hpp
	include/chig/LangModule.hpp
	include/chig/NodeInstance.hpp
	include/chig/NodeType.hpp
	include/chig/Result.hpp
)

set(CHIG_PRIVATE_FILES
	src/Context.cpp
	src/NodeInstance.cpp
	src/GraphFunction.cpp
        src/LangModule.cpp
        src/JsonModule.cpp
)

add_definitions("-D__STDC_LIMIT_MACROS")
add_definitions("-D__STDC_CONSTANT_MACROS")

add_library(chigcore ${CHIG_PUBLIC_FILES} ${CHIG_PRIVATE_FILES})

target_include_directories(chigcore
	PUBLIC
	${LLVM_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIR}
	include/
)

target_compile_features(chigcore PUBLIC cxx_constexpr cxx_rvalue_references cxx_variable_templates)

llvm_map_components_to_libnames(llvm_libs executionengine x86desc x86info x86codegen core irreader support bitwriter)


target_link_libraries(chigcore
	PUBLIC
	${llvm_libs} 
)

target_link_libraries(chigcore
PUBLIC
	debug ${Boost_SYSTEM_LIBRARY_DEBUG}
	optimized ${Boost_SYSTEM_LIBRARY_RELEASE}
	debug ${Boost_FILESYSTEM_LIBRARY_DEBUG}
	optimized ${Boost_FILESYSTEM_LIBRARY_RELEASE}
)
