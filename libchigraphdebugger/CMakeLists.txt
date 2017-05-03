# chigraphdebugger CMakeLists.txt

# Find LLDB library
###################

# Get the LLVM libdir where it should be
execute_process(COMMAND ${LLVM_CONFIG} --libdir OUTPUT_VARIABLE LLVM_LIB_DIR  OUTPUT_STRIP_TRAILING_WHITESPACE)

# search for it--for some reason it has a problem with symlinks so first look for liblldb.so.1
find_library(LLDB_LIBRARY liblldb.so.1 lldb HINTS ${LLVM_LIB_DIR} NO_DEFAULT_PATH)
message(STATUS "LLDB library: ${LLDB_LIBRARY}")


# Create the chigraphdebugger library
set(DEBUGGER_SRCS
	src/Debugger.cpp
)
set(DEBUGGER_HEADERS
	include/chi/Debugger.hpp
)
add_library(chigraphdebugger STATIC ${DEBUGGER_SRCS} ${DEBUGGER_HEADERS})

# Make sure dependencies have the include path
target_include_directories(chigraphdebugger
PUBLIC
	include
)

# Link to chigraph and LLDB
target_link_libraries(chigraphdebugger
PUBLIC 
	chigraph
	"${LLDB_LIBRARY}"
)

# Install the library and includes
install(TARGETS chigraphdebugger
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)
install(DIRECTORY include/chi DESTINATION include)


# Copy debug-server if we aren't on windows
if (NOT WIN32)
	
	# get LLVM version
	execute_process(COMMAND ${LLVM_CONFIG} --version OUTPUT_VARIABLE LLVM_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
	
	# get server name
	set(lldb_server_name lldb-server)
	
	# before 3.7 it's called lldb-gdbserver
	if (LLVM_VERSION VERSION_LESS "3.7.0")
		set(lldb_server_name lldb-gdbserver)
	endif()
	
	# on apple it's always been called debugserver
	if (APPLE)
		set(lldb_server_name debugserver)
	endif()
	
	# get the LLVM bindir, where lldb-server/lldb-gdbserver/debugserver is
	execute_process(COMMAND ${LLVM_CONFIG} --bindir OUTPUT_VARIABLE LLVM_BIN_DIR  OUTPUT_STRIP_TRAILING_WHITESPACE)

	# copy lldb-server
	add_custom_command(
	TARGET chigraphdebugger POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
		${LLVM_BIN_DIR}/${lldb_server_name}
		${CMAKE_BINARY_DIR}/bin/${lldb_server_name}
	BYPRODUCTS ${CMAKE_BINARY_DIR}/bin/${lldb_server_name}
	COMMENT "Copying lldb-server from ${LLVM_BIN_DIR}/${lldb_server_name} to ${CMAKE_BINARY_DIR}/bin/${lldb_server_name}"
	)
endif()
