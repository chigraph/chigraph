
set(CHIGC_PUBLIC_FILES

)

set(CHIGC_PRIVATE_FILES
	src/main.cpp
)

add_executable(chigc ${CHIGC_PUBLIC_FILES} ${CHIGC_PRIVATE_FILES})

target_link_libraries(chigc chig)

target_include_directories(chigc PUBLIC ../chig/include)
