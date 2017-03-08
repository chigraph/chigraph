
# get preprocessor flags
execute_process(COMMAND ${LLVM_CONFIG} --cppflags OUTPUT_VARIABLE LLVM_CXX_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
string(REPLACE " " ";" LLVM_CXX_FLAGS_LIST "${LLVM_CXX_FLAGS}")
message(STATUS "LLVM cxx flags: ${LLVM_CXX_FLAGS_LIST}")

execute_process(COMMAND ${LLVM_CONFIG} --ldflags OUTPUT_VARIABLE LLVM_LD_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
string(REPLACE " " ";" LLVM_LD_FLAGS_LIST "${LLVM_LD_FLAGS}")
message(STATUS "LLVM ld flags: ${LLVM_LD_FLAGS_LIST}")


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

