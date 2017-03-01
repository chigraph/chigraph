

include_directories(../chig/include)

add_definitions("-D__STDC_LIMIT_MACROS")
add_definitions("-D__STDC_CONSTANT_MACROS")

add_executable(chig main.cpp compile.cpp run.cpp interpret.cpp)

target_link_libraries(chig libchig ${llvm_libs} boost_program_options)
