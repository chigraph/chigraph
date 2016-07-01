
project(chig)

find_package(LLVM REQUIRED CONFIG)
find_package(Boost REQUIRED filesystem system)

include_directories(${LLVM_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

set(CHIGC_PUBLIC_FILES
	include/chig/json.hpp
	include/chig/context.hpp
	include/chig/built_in_types.hpp
	include/chig/node.hpp
)

set(CHIGC_PRIVATE_FILES
	src/chig/context.cpp
)

llvm_map_components_to_libnames(llvm_libs core irreader)

add_library(chig SHARED ${CHIGC_PUBLIC_FILES} ${CHIGC_PRIVATE_FILES})

target_link_libraries(chig 
PUBLIC
    ${llvm_libs}
	${Boost_LIBRARIES}
PRIVATE
)

target_include_directories(chig PUBLIC include/)
