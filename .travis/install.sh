#!/bin/bash

set -xe

if [ "$TRAVIS_OS_NAME" == "linux" ]; then

	sudo apt-get update
	sudo apt-get install qt${QT_VERSION:0:2}base qt${QT_VERSION:0:2}script qt${QT_VERSION:0:2}declarative qt${QT_VERSION:0:2}tools qt${QT_VERSION:0:2}x11extras qt${QT_VERSION:0:2}svg  ninja-build libedit-dev libxcb-keysyms1-dev libxml2-utils gcc-5 gcc-6 g++-5 g++-6 build-essential clang-3.7 clang-3.8 clang-3.9 llvm-${LLVM_VERSION}-dev python-pip
		
	
	source /opt/qt${QT_VERSION:0:2}/bin/qt${QT_VERSION:0:2}-env.sh
	
	export QTDIR=/opt/qt${QT_VERSION:0:2}
	export PATH=$QTDIR/bin:$PATH
	export LD_LIBRARY_PATH=$QTDIR/lib/x86_64-linux-gnu:$QTDIR/lib:$LD_LIBRARY_PATH
	
	./scripts/build_frameworks.sh -DCMAKE_BUILD_TYPE=$BUILD_TYPE -GNinja \
		-DCMAKE_CXX_COMPILER=$CXX_COMPILER -DCMAKE_C_COMPILER=$C_COMPILER
	

else

	brew install llvm wget qt5 bison gettext ninja
	./scripts/build_frameworks.sh \
		-DCMAKE_BUILD_TYPE=$BUILD_TYPE \
		-DCMAKE_PREFIX_PATH='/usr/local/opt/qt5;/usr/local/opt/gettext' \
		-DBISON_EXECUTABLE=/usr/local/opt/bison/bin/bison -GNinja
	
fi
	


