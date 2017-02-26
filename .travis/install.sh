#!/bin/bash

set -xe

if [ "$TRAVIS_OS_NAME" == "linux" ]; then
	./setup.py
else

	brew install cmake qt5 bison gettext ninja python3 || echo
	brew install llvm --with-clang
	/usr/local/bin/python3 ./setup.py
		
fi
	


