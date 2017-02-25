#!/bin/bash

set -xe

if [ "$TRAVIS_OS_NAME" == "linux" ]; then
	./setup.py
else

	brew install cmake qt5 bison gettext ninja || echo
	brew install llvm --with-clang
		
fi
	


