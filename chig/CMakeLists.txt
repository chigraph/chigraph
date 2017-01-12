

include_directories(../chig/include)

add_definitions("-D__STDC_LIMIT_MACROS")
add_definitions("-D__STDC_CONSTANT_MACROS")

add_executable(chig main.cpp compile.cpp run.cpp)

target_link_libraries(chig PUBLIC chigcore ${llvm_libs})
target_link_libraries(chig 
PUBLIC
	optimized ${Boost_PROGRAM_OPTIONS_LIBRARY_DEBUG}
	debug ${Boost_PROGRAM_OPTIONS_LIBRARY_RELEASE}
)
