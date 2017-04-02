#!/bin/bash

set -xe

if [ "$TRAVIS_OS_NAME" == "linux" ]; then
	
	if [ "$LLVM_VERSION" == "3.9" ] || [ "$LLVM_VERSION" == "4.0" ]; then
		sudo apt-get install liblldb-${LLVM_VERSION}-dev
	else
		sudo apt-get install lldb-${LLVM_VERSION}-dev
	fi

	bash ./setup.sh
else

	brew install cmake qt5 bison gettext ninja python3 || echo
	brew install llvm --with-clang
	bash ./setup.sh
		
fi
	


