find_package(Threads REQUIRED)

add_library(execstream exec-stream.cpp)
target_link_libraries(execstream PUBLIC Threads::Threads)
target_include_directories(execstream PUBLIC ./)

install(TARGETS execstream DESTINATION lib)
