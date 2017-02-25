#!/bin/bash

set -xe

if [ "$TRAVIS_OS_NAME" == "linux" ]; then
	./setup.py
	find . -maxdepth 2
else

	brew install cmake qt5 bison gettext ninja python3 || echo
	brew install llvm --with-clang
	./setup.py
		
fi
	


