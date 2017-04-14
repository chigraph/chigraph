#!/bin/bash

set -xe

if [ "$TRAVIS_OS_NAME" == "linux" ]; then
	
	sudo apt-get update && sudo apt-get install $PACKAGES \
		llvm-${LLVM_VERSION}-dev libclang-${LLVM_VERSION}-dev \
		libclang-common-${LLVM_VERSION}-dev libgit2 ninja-build \
		libedit-dev libxml2-utils libudev-dev texinfo build-essential
	
	if [ "$LLVM_VERSION" == "3.9" ] || [ "$LLVM_VERSION" == "4.0" ]; then
		sudo apt-get install liblldb-${LLVM_VERSION}-dev
	else
		sudo apt-get install lldb-${LLVM_VERSION}-dev
	fi
else

	brew install cmake qt5 bison gettext ninja python3 || echo
	brew install llvm --with-clang
		
fi
	


