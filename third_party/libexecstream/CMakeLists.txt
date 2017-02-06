project(libexecstream)
cmake_minimum_required(VERSION 2.8)

option(LIBEXECSTREAM_SHARED "Build shared library" OFF)
add_subdirectory(src)
if(BUILD_TESTS)
  add_subdirectory(test)
endif()
