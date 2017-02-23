#!/bin/bash

set -xe

if [ "$TRAVIS_OS_NAME" == "linux" ]; then
	./setup.py
else

	brew install cmake qt5 bison gettext ninja || echo
	brew install llvm --with-clang
	./scripts/build_frameworks.sh Debug "-DCMAKE_BUILD_TYPE=$BUILD_TYPE -DCMAKE_PREFIX_PATH='/usr/local/opt/qt5;/usr/local/opt/gettext' -DBISON_EXECUTABLE=/usr/local/opt/bison/bin/bison -GNinja"
	
fi
	


