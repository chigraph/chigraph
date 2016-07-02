

find_package(LLVM REQUIRED)

set(CHIG_PUBLIC_FILES
	include/chig/NodeType.h
	include/chig/Context.h
	include/chig/Module.h
)

set(CHIG_PRIVATE_FILES
	src/Context.c
	src/Module.c
)

# llvm-c seems to need these
add_definitions("-D__STDC_LIMIT_MACROS")
add_definitions("-D__STDC_CONSTANT_MACROS")

add_library(chig SHARED ${CHIG_PUBLIC_FILES} ${CHIG_PRIVATE_FILES})

target_include_directories(chig
	PUBLIC
	${LLVM_INCLUDE_DIRS}
	include/
)

llvm_map_components_to_libnames(llvm_libs core irreader)


target_link_libraries(chig
	PUBLIC
	${llvm_libs}
)
