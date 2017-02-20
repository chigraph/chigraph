#!/bin/bash

set -xe

if [ "$TRAVIS_OS_NAME" == "linux" ]; then
	
	./scripts/build_frameworks.sh -DCMAKE_BUILD_TYPE=$BUILD_TYPE -GNinja \
		-DCMAKE_CXX_COMPILER=$CXX_COMPILER -DCMAKE_C_COMPILER=$C_COMPILER
	

else

	brew install llvm cmake qt5 bison gettext ninja || echo
	./scripts/build_frameworks.sh \
		-DCMAKE_BUILD_TYPE=$BUILD_TYPE \
		-DCMAKE_PREFIX_PATH='/usr/local/opt/qt5;/usr/local/opt/gettext' \
		-DBISON_EXECUTABLE=/usr/local/opt/bison/bin/bison -GNinja
	
fi
	


