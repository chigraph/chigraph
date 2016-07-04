find_package(LLVM REQUIRED)

set(CHIG_PUBLIC_FILES
	include/chig/NodeType.hpp
	include/chig/Context.hpp
	include/chig/ImportedModule.hpp
)

set(CHIG_PRIVATE_FILES
	src/Context.cpp
	src/ImportedModule.cpp
)

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
