
find_package(Threads REQUIRED)

add_library(libchigraphexecstream STATIC exec-stream.cpp)
target_include_directories(libchigraphexecstream PUBLIC "../include")
target_link_libraries(libchigraphexecstream PUBLIC ${CMAKE_THREAD_LIBS_INIT})


install(TARGETS libchigraphexecstream 
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)


if(NOT MSVC)
    target_compile_options(libchigraphexecstream PRIVATE -fPIC)
endif()
