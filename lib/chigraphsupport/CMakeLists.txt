
set (CHIGRAPH_SUPPORT_HEADERS
	include/chi/Support/LibCLocator.hpp
	include/chi/Support/json.hpp
	include/chi/Support/Fwd.hpp
	include/chi/Support/HashUuid.hpp
	include/chi/Support/Subprocess.hpp
	include/chi/Support/Result.hpp
	include/chi/Support/HashFilesystemPath.hpp
	include/chi/Support/Flags.hpp
)

set(CHIGRAPH_SUPPORT_SRCS
	src/LibCLocator.cpp
	src/Result.cpp
	src/Subprocess.cpp
)

add_library(chigraphsupport ${CHIGRAPH_SUPPORT_HEADERS} ${CHIGRAPH_SUPPORT_SRCS})

find_package(Threads REQUIRED) # subprocess uses threads
target_link_libraries(chigraphsupport PUBLIC ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})

target_include_directories(chigraphsupport PUBLIC include/ ${Boost_INCLUDE_DIRS})

