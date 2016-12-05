

include_directories(../chig/include)
include_directories(../third_party/libgit2/include)

add_definitions("-D__STDC_LIMIT_MACROS")
add_definitions("-D__STDC_CONSTANT_MACROS")

add_executable(chig main.cpp compile.cpp run.cpp get.cpp)

target_link_libraries(chig PUBLIC chigcore git2)
target_link_libraries(chig 
PUBLIC
	optimized ${Boost_PROGRAM_OPTIONS_LIBRARY_DEBUG}
	debug ${Boost_PROGRAM_OPTIONS_LIBRARY_RELEASE}
)
