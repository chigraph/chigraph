
set (CHIGRAPH_SUPPORT_HEADERS
	include/chi/Support/ExecutablePath.hpp
	include/chi/Support/FindProgram.hpp
	include/chi/Support/Flags.hpp
	include/chi/Support/Fwd.hpp
	include/chi/Support/HashFilesystemPath.hpp
	include/chi/Support/json.hpp
	include/chi/Support/LibCLocator.hpp
	include/chi/Support/Result.hpp
	include/chi/Support/Subprocess.hpp
	include/chi/Support/TempFile.hpp
	include/chi/Support/Uuid.hpp
)

set(CHIGRAPH_SUPPORT_SRCS
	src/ExecutablePath.cpp
	src/FindProgram.cpp
	src/LibCLocator.cpp
	src/Result.cpp
	src/Subprocess.cpp
	src/TempFile.cpp
	src/Uuid.cpp
)

add_library(chigraphsupport STATIC ${CHIGRAPH_SUPPORT_HEADERS} ${CHIGRAPH_SUPPORT_SRCS})

find_package(Threads REQUIRED) # subprocess uses threads
target_link_libraries(chigraphsupport PUBLIC ${CMAKE_THREAD_LIBS_INIT})

target_include_directories(chigraphsupport 
	PUBLIC include/ 
	PRIVATE ${Boost_INCLUDE_DIRS}
)

set_property(TARGET chigraphsupport PROPERTY CXX_STANDARD 17)
set_property(TARGET chigraphsupport PROPERTY CXX_STANDARD_REQUIRED ON)

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "(Clang)|(AppleClang)|(GNU)")
	target_compile_options(chigraphsupport PUBLIC -Wall -Wextra -pedantic-errors -Werror -Wno-unused-parameter -Wno-missing-field-initializers)
endif()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	target_link_libraries(chigraphsupport PUBLIC "bcrypt.lib") # boost.uuid expects to be linked to this
endif()

install(TARGETS chigraphsupport
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)

install(DIRECTORY include/chi DESTINATION include)
