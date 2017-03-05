

set(DEBUGGER_SRCS
	src/Debugger.cpp
)

set(DEBUGGER_HEADERS
	include/chig/Debugger.hpp
)

add_library(chigdebugger STATIC ${DEBUGGER_SRCS} ${DEBUGGER_HEADERS})

target_include_directories(chigdebugger
PUBLIC
	include
)

target_link_libraries(chigdebugger
PUBLIC 
	libchig
	lldb
)

