
add_executable(errorchecker errorchecker.cpp)

target_link_libraries(errorchecker PUBLIC chig)


# Errors
file(GLOB ERROR_TESTS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.chigfn")

foreach(err_file in ${ERROR_TESTS})
	
	
	
	# get error code
	string(FIND ${err_file} "." dotloc)
	string(SUBSTRING ${err_file} 0 ${dotloc} error_code)
	
	add_test(NAME ${error_code}_test
	COMMAND chigc ${err_file} | errorchecker ${error_code}
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
	
endforeach()
