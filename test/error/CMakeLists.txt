
# Errors
file(GLOB ERROR_TESTS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.chigfn")


foreach(JSON_FILE ${ERROR_TESTS})
	
	# get error code
	string(FIND ${JSON_FILE} "." dotloc)
	string(SUBSTRING ${JSON_FILE} 0 ${dotloc} EXPECTED_ERR)
	
	set(FULL_JSON_FILE ${CMAKE_CURRENT_SOURCE_DIR}/${JSON_FILE})
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/errorchecker.cpp.in ${CMAKE_CURRENT_BINARY_DIR}/${JSON_FILE}.cpp)
	add_executable(${JSON_FILE}_test ${CMAKE_CURRENT_BINARY_DIR}/${JSON_FILE}.cpp)
	
	target_link_libraries(${JSON_FILE}_test PUBLIC chig)
	
	add_test(NAME ${JSON_FILE}_test
		COMMAND ${JSON_FILE}_test
	)
	
endforeach()
