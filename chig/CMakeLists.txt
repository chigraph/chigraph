
project(chig)

find_package(LLVM REQUIRED CONFIG)
find_package(Boost REQUIRED)

include_directories(${LLVM_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

set(CHIGC_PUBLIC_FILES
	include/chig/connection.hpp
	include/chig/node_instance.hpp
	include/chig/node_type.hpp
)

set(CHIGC_PRIVATE_FILES
	
)

llvm_map_components_to_libnames(llvm_libs core)

add_library(chig SHARED ${CHIGC_PUBLIC_FILES} ${CHIGC_PRIVATE_FILES})

target_link_libraries(chig 
PRIVATE
	${llvm_libs}
)

target_include_directories(chig PUBLIC include/)
