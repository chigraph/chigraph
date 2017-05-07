
find_package(Threads REQUIRED)

add_library(libchigraphexecstream STATIC exec-stream.cpp)
target_include_directories(libexecstream PUBLIC "../include")
target_link_libraries(libexecstream PUBLIC ${CMAKE_THREAD_LIBS_INIT})


install(TARGETS libexecstream 
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)


if(NOT MSVC)
    target_compile_options(libexecstream PRIVATE -fPIC)
endif()
