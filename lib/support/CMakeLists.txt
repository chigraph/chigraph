
set (CHIGRAPH_SUPPORT_HEADERS
	include/chi/Support/ExecutablePath.hpp
	include/chi/Support/FindProgram.hpp
	include/chi/Support/Flags.hpp
	include/chi/Support/Fwd.hpp
	include/chi/Support/HashFilesystemPath.hpp
	include/chi/Support/HashUuid.hpp
	include/chi/Support/json.hpp
	include/chi/Support/LibCLocator.hpp
	include/chi/Support/Result.hpp
	include/chi/Support/Subprocess.hpp
	include/chi/Support/TempFile.hpp
)

set(CHIGRAPH_SUPPORT_SRCS
	src/ExecutablePath.cpp
	src/FindProgram.cpp
	src/LibCLocator.cpp
	src/Result.cpp
	src/Subprocess.cpp
	src/TempFile.cpp
)

add_library(chigraphsupport STATIC ${CHIGRAPH_SUPPORT_HEADERS} ${CHIGRAPH_SUPPORT_SRCS})

find_package(Threads REQUIRED) # subprocess uses threads
target_link_libraries(chigraphsupport PUBLIC ${CMAKE_THREAD_LIBS_INIT})

target_include_directories(chigraphsupport PUBLIC include/ ${Boost_INCLUDE_DIRS})

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR 
		CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang" OR
		CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_compile_options(chigraphsupport PUBLIC -std=c++17)
	target_compile_options(chigraphsupport PUBLIC -Wall -Wextra -pedantic-errors -Werror -Wno-unused-parameter -Wno-missing-field-initializers)
endif()


install(TARGETS chigraphsupport
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)

install(DIRECTORY include/chi DESTINATION include)
