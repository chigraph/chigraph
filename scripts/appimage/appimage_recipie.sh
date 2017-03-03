#!/bin/bash

set -xe

# acquire linuxdeployqt
cd /
rm -Rf /linuxdeployqt
if [ ! -d AppImageKit ] ; then
  git clone  --depth 1 https://github.com/probonopd/linuxdeployqt.git /linuxdeployqt
fi

cd /linuxdeployqt/
/usr/local/Qt-5.8.0/bin/qmake linuxdeployqt.pro
make
cd /

# enable newer compiler
source /opt/rh/devtoolset-4/enable

QTVERSION=5.8.0
QVERSION_SHORT=5.8
QTDIR=/usr/local/Qt-${QTVERSION}/

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
/chigraph/scripts/build_frameworks.sh Release "-GNinja -DCMAKE_PREFIX_PATH='/chigraph.appdir/usr;$QTDIR' -DCMAKE_INSTALL_PREFIX=/chigraph.appdir/usr"

# build chigraph
cd /chigraph
rm -rf build
mkdir -p build
cd build
cmake3 .. -GNinja -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="/chigraph.appdir/usr;/opt/llvm;$QTDIR" -DCMAKE_INSTALL_PREFIX='/usr' -DCG_LINK_FFI=OFF
ninja DESTDIR=/chigraph.appdir install

# remove pointless stuff
cd /chigraph.appdir/
rm -rf ./usr/include
find . -name '*.a' -exec rm {} \;

cp /chigraph/scripts/appimage/chiggui.desktop /chigraph.appdir/

unset QTDIR # not sure why we do this
/linuxdeployqt/linuxdeployqt/linuxdeployqt /chigraph.appdir/chiggui.dekstop -bundle-non-qt-libs
/linuxdeployqt/linuxdeployqt/linuxdeployqt /chigraph.appdir/chiggui.dekstop -appimage

