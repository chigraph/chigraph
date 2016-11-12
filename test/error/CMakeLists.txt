add_executable(error_tester errorchecker.cpp)
target_link_libraries(error_tester PUBLIC chig)

# Errors
file(GLOB ERROR_TESTS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.chigfn")


foreach(JSON_FILE ${ERROR_TESTS})
	
	# get error code
	string(FIND ${JSON_FILE} "." dotloc)
	string(SUBSTRING ${JSON_FILE} 0 ${dotloc} EXPECTED_ERR)
	
	set(FULL_JSON_FILE ${CMAKE_CURRENT_SOURCE_DIR}/${JSON_FILE})
	
	add_test(NAME ${JSON_FILE}_test
		COMMAND error_tester ${JSON_FILE} ${EXPECTED_ERR}
	)
	
endforeach()
