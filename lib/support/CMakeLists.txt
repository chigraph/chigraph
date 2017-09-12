
set (CHIGRAPH_SUPPORT_HEADERS
	include/chi/Support/LibCLocator.hpp
	include/chi/Support/json.hpp
	include/chi/Support/Fwd.hpp
	include/chi/Support/HashUuid.hpp
	include/chi/Support/Subprocess.hpp
	include/chi/Support/Result.hpp
	include/chi/Support/HashFilesystemPath.hpp
	include/chi/Support/Flags.hpp
	include/chi/Support/ExecutablePath.hpp
)

set(CHIGRAPH_SUPPORT_SRCS
	src/LibCLocator.cpp
	src/Result.cpp
	src/Subprocess.cpp
	src/ExecutablePath.cpp
)

add_library(chigraphsupport STATIC ${CHIGRAPH_SUPPORT_HEADERS} ${CHIGRAPH_SUPPORT_SRCS})

find_package(Threads REQUIRED) # subprocess uses threads
target_link_libraries(chigraphsupport PUBLIC ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})

target_include_directories(chigraphsupport PUBLIC include/ ${Boost_INCLUDE_DIRS})

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR 
		CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang" OR
		CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_compile_options(chigraphsupport PUBLIC -std=c++14)
	target_compile_options(chigraphsupport PUBLIC -Wall -Wextra -pedantic-errors -Werror -Wno-unused-parameter -Wno-missing-field-initializers)
endif()

