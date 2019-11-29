# chigraphfetcher CMakeLists.txt

set(CHI_FETCHER_SRCS
	src/Fetcher.cpp
)

set(CHI_FETCHER_HDRS
	include/chi/Fetcher/Fetcher.hpp
)

add_library(chigraphfetcher STATIC ${CHI_FETCHER_SRCS} ${CHI_FETCHER_HDRS})

set_property(TARGET chigraphfetcher PROPERTY CXX_STANDARD 17)
set_property(TARGET chigraphfetcher PROPERTY CXX_STANDARD_REQUIRED ON)

target_include_directories(chigraphfetcher
PUBLIC
	include/
PRIVATE
	${LIBGIT2_INCLUDE_DIR}
)

target_compile_options(chigraphfetcher
PRIVATE
	${LIBGIT2_DEFINITIONS}
)

target_link_libraries(chigraphfetcher 
PUBLIC
	chigraphsupport
PRIVATE
	${LIBGIT2_LIBRARIES}
)


install(TARGETS chigraphfetcher
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)

install(DIRECTORY include/chi DESTINATION include)

