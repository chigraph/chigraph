
set(CHI_SRCS
	main.cpp
	compile.cpp
	run.cpp
	interpret.cpp
	init.cpp
)

if (CG_BUILD_FETCHER) 
	list(APPEND CHI_SRCS get.cpp)
else () 
	list(APPEND CHI_SRCS get_nop.cpp)
endif()

add_executable(chi ${CHI_SRCS})

target_link_libraries(chi chigraphcore ${Boost_PROGRAM_OPTIONS_LIBRARY})

if (CG_BUILD_FETCHER) 
	target_link_libraries(chi chigraphfetcher)
endif()

install(TARGETS chi DESTINATION bin)
