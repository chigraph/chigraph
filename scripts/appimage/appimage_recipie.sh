#!/bin/bash

set -xe

# acquire appimagekit
cd /
wget https://github.com/probonopd/AppImageKit/releases/download/8/appimagetool-x86_64.AppImage -O appimagetool
chmod a+x appimagetool

# enable newer compiler
source /opt/rh/devtoolset-4/enable

QTVERSION=5.8.0
QVERSION_SHORT=5.8
QTDIR=/usr/local/Qt-${QTVERSION}/

# install chigraph
cd /
rm -rf /chigraph || true
git clone https://github.com/chigraph/chigraph --depth=1 

# prepare the appdir
mkdir -p /chigraph.appdir/usr
mkdir -p /chigraph.appdir/usr/lib
cd  /chigraph.appdir/usr
rm -rf lib64 || true
ln -s lib lib64

# force use of cmake3
mv /usr/bin/cmake /usr/bin/cmake-old
ln -s /usr/bin/cmake3 /usr/bin/cmake

# build KF5
export PATH=/opt/rh/python27/root/usr/bin/:$PATH
/chigraph/scripts/build_frameworks.sh Release "-DCMAKE_PREFIX_PATH='/chigraph.appdir/usr;$QTDIR' -DCMAKE_INSTALL_PREFIX=/chigraph.appdir/usr"

# build chigraph
cd /chigraph
mkdir -p build
cd build
cmake3 .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH='/chigraph.appdir/usr;/opt/llvm' -DCMAKE_INSTALL_PREFIX='/chigraph.apdir/usr'
make -j8 install

# remove pointless stuff
cd /chigraph.appdir/
rm -rf ./usr/include
find . -name '*.a' -exec rm {} \;

