
if(LIBEXECSTREAM_SHARED)
    set(libtype SHARED)
else()
    set(libtype)
endif()

find_package(Threads REQUIRED)

add_library(libexecstream ${libtype} exec-stream.cpp)
target_include_directories(libexecstream PUBLIC "../include")
target_link_libraries(libexecstream PUBLIC ${CMAKE_THREAD_LIBS_INIT})

if(NOT MSVC)
    target_compile_options(libexecstream PRIVATE -fPIC)
endif()
