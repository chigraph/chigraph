#!/bin/bash

set -xe

SCRIPTSDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd $SCRIPTSDIR/..

#rm -rf build
#mkdir -p build
cd build

cmake .. $@ -DCMAKE_BUILD_TYPE=Release
make -j`nproc`

wget 'https://github.com/probonopd/AppImageKit/releases/download/continuous/appimagetool-i686.AppImage' -O appimagetool
export PATH=$PATH:`pwd`

wget 'https://github.com/probonopd/linuxdeployqt/releases/download/2/linuxdeployqt-2-x86_64.AppImage' -O linuxdeployqt
chmod a+x linuxdeployqt

cp -r $SCRIPTSDIR/appimage/* ./gui/
LD_LIBRARY_PATH=`pwd`/lib:`pwd`/third_party/tiny-process-library:`pwd`/libchig:$LD_LIBRARY_PATH ./linuxdeployqt ./gui/chiggui -appimage -verbose=2 -always-overwrite
