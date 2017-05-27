# Build all the examples

set (EXAMPLE_NAMES Subprocess Result)

foreach (example ${EXAMPLE_NAMES})
	string(TOLOWER ${example} lowerexample)

	add_executable(${lowerexample} ${example}Example.cpp)
	target_link_libraries(${lowerexample} chigraph)
	
	
	# remove them from all if requested
	if (NOT CG_BUILD_EXAMPLES)
		set_target_properties(${lowerexample} PROPERTIES 
			EXCLUDE_FROM_ALL 1 
			EXCLUDE_FROM_DEFAULT_BUILD 1
		)
	endif()

	
endforeach()
