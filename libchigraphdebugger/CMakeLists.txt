
# get preprocessor flags
execute_process(COMMAND ${LLVM_CONFIG} --cppflags OUTPUT_VARIABLE LLVM_CXX_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
string(REPLACE " " ";" LLVM_CXX_FLAGS_LIST "${LLVM_CXX_FLAGS}")

execute_process(COMMAND ${LLVM_CONFIG} --ldflags OUTPUT_VARIABLE LLVM_LD_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
string(REPLACE " " ";" LLVM_LD_FLAGS_LIST "${LLVM_LD_FLAGS}")

execute_process(COMMAND ${LLVM_CONFIG} --libdir OUTPUT_VARIABLE LLVM_LIB_DIR  OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(COMMAND ${LLVM_CONFIG} --version OUTPUT_VARIABLE LLVM_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)

string(SUBSTRING ${LLVM_VERSION} 0 3 LLVM_VERSION_SHORT)


find_library(LLDB_LIBRARY liblldb.so.1 lldb HINTS ${LLVM_LIB_DIR})
message(STATUS "LLDB library: ${LLDB_LIBRARY}")

set(DEBUGGER_SRCS
	src/Debugger.cpp
)

set(DEBUGGER_HEADERS
	include/chi/Debugger.hpp
)

add_library(chigraphdebugger STATIC ${DEBUGGER_SRCS} ${DEBUGGER_HEADERS})

target_compile_options(chigraphdebugger 
PUBLIC
	${LLVM_CXX_FLAGS_LIST}
)

target_include_directories(chigraphdebugger
PUBLIC
	include
)

target_link_libraries(chigraphdebugger
PUBLIC 
	chigraph
	${LLVM_LD_FLAGS_LIST}
	"${LLDB_LIBRARY}"
)

