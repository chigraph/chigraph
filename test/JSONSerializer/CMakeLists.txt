
add_executable(JSONSerializer main.cpp)
target_link_libraries(JSONSerializer chig)

add_test(JSONSerializer JSONSerializer)
