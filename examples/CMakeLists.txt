# Build all the examples

add_executable(subprocess_example SubprocessExample.cpp)
target_link_libraries(subprocess_example chigraph)


# remove them from all if requested
if (NOT CG_BUILD_EXAMPLES)
	set_target_properties(subprocess_example PROPERTIES EXCLUDE_FROM_ALL 1 EXCLUDE_FROM_DEFAULT_BUILD 1)
endif()
