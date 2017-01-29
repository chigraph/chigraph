#!/bin/bash

set -xe

mkdir build
cd build

if [ "$TRAVIS_OS_NAME" == "linux" ]; then

	covflags=""
	if [ -n "$TEST_COV" ]; then 

		covflags="-DCMAKE_CXX_FLAGS='-fprofile-arcs -ftest-coverage'"

	fi 

	cmake .. -DCMAKE_BUILD_TYPE=$BUILD_TYPE \ 
		-DCMAKE_CXX_COMPILER=$CXX_COMPILER \ 
		-DCMAKE_C_COMPILER=$C_COMPILER \ 
		-DLLVM_CONFIG=/usr/lib/llvm-${LLVM_VERSION}/bin/llvm-config \ 
		-DCLANG_EXE=`which clang-${LLVM_VERSION}` \ 
		-DLLI_EXE=`which lli-${LLVM_VERSION}` \ 
		-GNinja $covflags \ 

	ninja
	CTEST_OUTPUT_ON_FAILURE=1 ninja test

	if [ -n "$TEST_COV" ]; then 
		pip install --user cpp-coveralls 
		coveralls --exclude ../test --exclude ../doc --exclude ../gui --include ../libchig --include ../chig --exclude ../libchig/include/json.hpp
	fi

else 
	
	cmake .. \
		-DCMAKE_PREFIX_PATH='/usr/local/opt/qt5/;/usr/local/opt/gettext' \
		-DCMAKE_BUILD_TYPE=Debug \
		-DLLVM_CONFIG=/usr/local/opt/llvm/bin/llvm-config \
		-DLLI_EXE=/usr/local/opt/llvm/bin/lli -GNinja
	ninja
	# ninja test
	
fi
