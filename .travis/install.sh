#!/bin/bash

set -xe

if [ "$TRAVIS_OS_NAME" != "linux" ]; then

	brew install cmake ninja || echo
	brew install llvm --with-clang
	
fi
	


