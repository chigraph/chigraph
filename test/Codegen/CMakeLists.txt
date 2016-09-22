
add_executable(Codegen main.cpp)
target_link_libraries(Codegen chig)

add_test(Codegen Codegen)
