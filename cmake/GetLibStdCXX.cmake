
include(ProcessorCount)
ProcessorCount(N)

if(UNIX AND NOT APPLE AND NOT WIN32)

	if (NOT EXISTS ${CMAKE_BINARY_DIR}/glibc.tar.xz)
		message(STATUS "Downloading glibc...")
		file(DOWNLOAD http://mirrors.ocf.berkeley.edu/gnu/libc/glibc-2.19.tar.xz ${CMAKE_BINARY_DIR}/glibc.tar.xz SHOW_PROGRESS)
		
	endif()
	if(NOT EXISTS ${CMAKE_BINARY_DIR}/glibc-2.19)
		message(STATUS "Extracting glibc...")
		execute_process(COMMAND ${CMAKE_COMMAND} -E tar xf glibc.tar.xz WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
			
	endif()
	
	if (NOT EXISTS ${CMAKE_BINARY_DIR}/glibc-2.19/build)
		message(STATUS "Building glibc")
		file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/glibc-2.19/build)
		execute_process(COMMAND ../configure --prefix=${CMAKE_BINARY_DIR}/lib/chigraph/stdlib WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/glibc-2.19/build)
		
		execute_process(COMMAND make -j${N} WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/glibc-2.19/build)
		execute_process(COMMAND make install WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/glibc-2.19/build)
	endif()
	
# 
# 	find_package(Git REQUIRED)
# 	
# 	message(STATUS "Git found at ${GIT_EXECUTABLE}")
# 	message(STATUS "Cloning libstdc++...")
# 	execute_process(COMMAND ${GIT_EXECUTABLE} clone git://gcc.gnu.org/git/gcc.git --branch=gcc-6-branch --depth=1 WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
# 
# 	message(STATUS "Building libstdc++...")
# 	file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/gcc/build)
# 	execute_process(COMMAND ../libstdc++-v3/configure --prefix=${CMAKE_BINARY_DIR}/lib/chigraph/stdlib WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/gcc/build)
# 	execute_process(COMMAND make WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/gcc/build)
# 	execute_process(COMMAND make install WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/gcc/build)
# 	
	

elseif(APPLE)
	
	message(STATUS "Downloading libc++abi...")
	file(DOWNLOAD http://releases.llvm.org/3.9.1/libcxxabi-3.9.1.src.tar.xz ${CMAKE_BINARY_DIR}/libcxxabi.tar.xz SHOW_PROGRESS)
	
	message(STATUS "Extracting libc++abi...")
	execute_process(COMMAND ${CMAKE_COMMAND} -E tar xf libcxxabi.tar.xz WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
	
	message(STATUS "Building libc++abi...")
	file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/libcxxabi-3.9.1.src/build)
	execute_process(COMMAND ${CMAKE_COMMAND} ../ -G${CMAKE_GENERATOR} -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/lib/chigraph/stdlib -DCMAKE_BUILD_TYPE=Release WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/libcxxabi-3.9.1.src/build)
	execute_process(COMMAND ${CMAKE_COMMAND} --build . --target=install WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/libcxxabi-3.9.1.src/build)
	
	message(STATUS "Downloading libc++...")
	file(DOWNLOAD http://releases.llvm.org/3.9.1/libcxx-3.9.1.src.tar.xz ${CMAKE_BINARY_DIR}/libcxx.tar.xz SHOW_PROGRESS)
	
	message(STATUS "Extracting libc++...")
	execute_process(COMMAND ${CMAKE_COMMAND} -E tar xf libcxx.tar.xz WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
	
	message(STATUS "Building libc++abi...")
	file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/libcxx-3.9.1.src/build)
	execute_process(COMMAND ${CMAKE_COMMAND} ../ -G${CMAKE_GENERATOR} -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/lib/chigraph/stdlib -DCMAKE_PREFIX_PATH=${CMAKE_BINARY_DIR}/libcxx -DCMAKE_BUILD_TYPE=Release WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/libcxx-3.9.1.src/build)
	execute_process(COMMAND ${CMAKE_COMMAND} --build . --target=install WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/libcxx-3.9.1.src/build)
	
	
	
endif()
