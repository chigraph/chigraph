# Build all the examples

set (EXAMPLE_NAMES Subprocess Result)

foreach (example ${EXAMPLE_NAMES})
	string(TOLOWER ${example} lowerexample)
	
	set(target_name ${lowerexample}_example)

	add_executable(${target_name} ${example}Example.cpp)
	target_link_libraries(${target_name} chigraphcore)
	
	
	# remove them from all if requested
	if (NOT CG_BUILD_EXAMPLES)
		set_target_properties(${target_name} PROPERTIES 
			EXCLUDE_FROM_ALL 1 
			EXCLUDE_FROM_DEFAULT_BUILD 1
		)
	endif()

	
endforeach()
