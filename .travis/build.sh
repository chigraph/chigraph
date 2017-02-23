#!/bin/bash

set -xe

if [ "$TRAVIS_OS_NAME" == "linux" ]; then

	if [ -n "$TEST_COV" ]; then 

		cmake . -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
			-DCMAKE_CXX_COMPILER=$CXX_COMPILER \
			-DCMAKE_C_COMPILER=$C_COMPILER \
			-GNinja -DCMAKE_CXX_FLAGS='--coverage'
	else

		cmake . -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
			-DCMAKE_CXX_COMPILER=$CXX_COMPILER \
			-DCMAKE_C_COMPILER=$C_COMPILER \
			-GNinja
			
	fi

	ninja
	CTEST_OUTPUT_ON_FAILURE=1 ninja test

	if [ -n "$TEST_COV" ]; then 
		pip install --user cpp-coveralls 
		coveralls --exclude /usr --exclude third_party --exclude gui --exclude test --exclude build --include libchig --include chig --exclude libchig/include/chig/json.hpp    

	fi

else 
	
	cmake . \
		-DCMAKE_PREFIX_PATH='/usr/local/opt/qt5/;/usr/local/opt/gettext' \
		-DCMAKE_BUILD_TYPE=Debug \
		-DLLVM_CONFIG=/usr/local/opt/llvm/bin/llvm-config \
		-DLLI_EXE=/usr/local/opt/llvm/bin/lli -GNinja
	ninja
	# ninja test
	
fi
