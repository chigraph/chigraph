

include_directories(../chig/include)


add_definitions("-D__STDC_LIMIT_MACROS")
add_definitions("-D__STDC_CONSTANT_MACROS")

add_executable(chigc main.cpp)

target_link_libraries(chigc PUBLIC chig)
