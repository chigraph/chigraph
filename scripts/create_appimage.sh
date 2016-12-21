#!/bin/bash

set -xe

SCRIPTSDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd $SCRIPTSDIR/..

rm -rf build
mkdir -p build
cd build

cmake .. $@
make

wget 'https://github.com/probonopd/linuxdeployqt/releases/download/2/linuxdeployqt-2-x86_64.AppImage' -O linuxdeployqt
chmod a+x linuxdeployqt

cp $SCRIPTSDIR/appimage ./gui/
./linuxdeployqt ./gui/chiggui
