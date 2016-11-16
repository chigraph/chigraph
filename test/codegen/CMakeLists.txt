#add_executable(codegen_tester codegentester.cpp)
#target_link_libraries(codegen_tester PUBLIC chigcore)
#target_link_libraries(codegen_tester PUBLIC execstream)

# These don't follow chigfn or chigmod specs, so don't use that extension
file(GLOB CODEGEN_TESTS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.json") 

foreach(JSON_FILE ${CODEGEN_TESTS})
	
	# get error code
	
	set(FULL_JSON_FILE ${CMAKE_CURRENT_SOURCE_DIR}/${JSON_FILE})
	
	add_test(NAME ${JSON_FILE}_test
		COMMAND codegen_tester ${FULL_JSON_FILE}
	)
	
endforeach()

