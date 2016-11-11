
add_executable(errorchecker errorchecker.cpp)

target_link_libraries(errorchecker PUBLIC chig)


# Errors
file(GLOB ERROR_TESTS ./ "E[1-10]*\\.chigfn")

foreach(err_file in ${ERROR_TESTS})
	
	# get error code
	string(FIND "." ${err_file} dotloc)
	string(SUBSTRING ${err_file} 0 ${dotloc} error_code)
	
	add_custom_target(${error_code}_gen
		COMMAND chigc ${err_file} > ${CMAKE_BINARY_DIR}/${error_code}_outputed.json 
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${err_file})
	
	add_test(NAME ${error_code}_test
	COMMAND errorchecker ${CMAKE_BINARY_DIR}/${error_code}_outputed.json ${error_code})
	
	add_dependencies(${error_code}_test ${error_code}_gen)
	
	
endforeach()
