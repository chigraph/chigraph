
add_subdirectory(runtime)
add_subdirectory(support)
add_subdirectory(core)

if (CG_BUILD_DEBUGGER)
 	add_subdirectory(debugger)
endif()

if (CG_BUILD_FETCHER)
	add_subdirectory(fetcher)
endif()

