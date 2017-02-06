project(libexecstream_conan_package_test)

cmake_minimum_required(VERSION 2.8)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()
include(${CONAN_CMAKE-UTILS_ROOT}/conan.cmake)

add_conan_library(libexecstream)

add_executable(example main.cpp)
target_link_libraries(example PRIVATE libexecstream-conan)
