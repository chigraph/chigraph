add_executable(codegen_tester codegentester.cpp)
target_link_libraries(codegen_tester PUBLIC chigraphcore)

set_property(TARGET codegen_tester PROPERTY CXX_STANDARD 17)
set_property(TARGET codegen_tester PROPERTY CXX_STANDARD_REQUIRED ON)


set(srcdir ${CMAKE_CURRENT_SOURCE_DIR}/workspace/src)
file(GLOB children RELATIVE  ${srcdir} ./workspace/src/*)
set(CODEGEN_TESTS "")
foreach(child ${children})
  if(IS_DIRECTORY ${srcdir}/${child})
    list(APPEND CODEGEN_TESTS ${child})
  endif()
endforeach()

foreach(MODULEPATH ${CODEGEN_TESTS})
	
	message("Found codegen test: ${MODULEPATH}")
	# get error code
	
	set(FULLMODULEPATH ${srcdir}/${MODULEPATH})
	
	add_test(NAME ${MODULEPATH}_test
		COMMAND codegen_tester ${FULLMODULEPATH}
		WORKING_DIRECTORY $<TARGET_FILE_DIR:codegen_tester>
	)
	
endforeach()

