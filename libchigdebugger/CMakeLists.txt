
# get preprocessor flags
execute_process(COMMAND ${LLVM_CONFIG} --cppflags OUTPUT_VARIABLE LLVM_CXX_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
string(REPLACE " " ";" LLVM_CXX_FLAGS_LIST "${LLVM_CXX_FLAGS}")

execute_process(COMMAND ${LLVM_CONFIG} --ldflags OUTPUT_VARIABLE LLVM_LD_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
string(REPLACE " " ";" LLVM_LD_FLAGS_LIST "${LLVM_LD_FLAGS}")

execute_process(COMMAND ${LLVM_CONFIG} --libdir OUTPUT_VARIABLE LLVM_LIB_DIR  OUTPUT_STRIP_TRAILING_WHITESPACE)


find_library(LLDB_LIBRARY lldb HINTS ${LLVM_LIB_DIR})
message(STATUS "LLDB library: ${LLDB_LIBRARY}")

set(DEBUGGER_SRCS
	src/Debugger.cpp
)

set(DEBUGGER_HEADERS
	include/chig/Debugger.hpp
)

add_library(chigdebugger STATIC ${DEBUGGER_SRCS} ${DEBUGGER_HEADERS})

target_compile_options(chigdebugger 
PUBLIC
	${LLVM_CXX_FLAGS_LIST}
)

target_include_directories(chigdebugger
PUBLIC
	include
)

target_link_libraries(chigdebugger
PUBLIC 
	libchig
	${LLVM_LD_FLAGS_LIST}
	${LLDB_LIBRARY}
)

