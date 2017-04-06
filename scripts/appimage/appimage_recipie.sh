#!/bin/bash

set -xe

sudo apt-get update
sudo apt-get install g++-6 llvm-3.9-dev libclang-3.9-dev liblldb-3.9-dev libclang-common-3.9-dev libgit2 qt58base qt58script qt58declarative qt58tools qt58x11extras qt58svg ninja-build libedit-dev libxcb-keysyms1-dev libxml2-utils libudev-dev texinfo build-essential 

QT_BASE_DIR=/opt/qt58
export QTDIR=$QT_BASE_DIR
export PATH=$QT_BASE_DIR/bin:$PATH

# acquire appimagetool
sudo wget https://github.com/probonopd/AppImageKit/releases/download/continuous/appimagetool-x86_64.AppImage -O /usr/bin/appimagetool
sudo chmod a+x /usr/bin/appimagetool

# acquire linuxdeployqt
sudo wget https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage -O /usr/bin/linuxdeployqt
sudo chmod a+x /usr/bin/linuxdeployqt

# prepare the appdir
mkdir -p ~/chigraph.appdir/usr
mkdir -p ~/chigraph.appdir/usr/lib
rm -rf ~/chigraph.appdir/usr/lib64 || true
ln -s ~/chigraph.appdir/usr/lib ~/chigraph.appdir/usr/lib64


# setup KF5
./setup.sh
rsync -raPq third_party/kf5-release/* ~/chigraph.appdir/usr/

# build chigraph
rm -rf build
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH='~/chigraph.appdir/usr' -DCMAKE_INSTALL_PREFIX='/usr' -DLLVM_CONFIG='/usr/lib/llvm-3.9/bin/llvm-config' -DCMAKE_CXX_COMPILER=g++-6 -DCMAKE_C_COMPILER=gcc-6
make -j8 DESTDIR=~/chigraph.appdir install

cd ..

# remove pointless stuff
rm -rf ~/chigraph.appdir/usr/include
find ~/chigraph.appdir/ -name '*.a' -exec rm {} \;

cp scripts/appimage/chigraphgui.desktop ~/chigraph.appdir/
cp scripts/appimage/chigraph.png ~/chigraph.appdir/

cp /usr/lib/llvm-3.9/bin/lldb-server ~/chigraph.appdir/usr/bin/

QT_BASE_DIR=/opt/qt58
export QTDIR=$QT_BASE_DIR
export PATH=$QT_BASE_DIR/bin:$PATH

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:`pwd`/third_party/kf5-release/lib64:$LD_LIBRARY_PATH:`pwd`/third_party/kf5-release/lib

linuxdeployqt ~/chigraph.appdir/usr/bin/chigraphgui -bundle-non-qt-libs
strip -s ~/chigraph.appdir/usr/bin/chigraphgui ~/chigraph.appdir/usr/bin/chi
linuxdeployqt ~/chigraph.appdir/usr/bin/chigraphgui -appimage

