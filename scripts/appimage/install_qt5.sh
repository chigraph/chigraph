#!/bin/bash

set -xe

mkdir -p /qt5
cd /qt5

ln -s /opt/llvm/bin/clang++ /usr/bin/clang++
ln -s /opt/llvm/bin/clang /usr/bin/clang

if [ ! -f qt-everywhere-opensource-src-${QTVERSION}.tar.xz ]; then
  wget http://download.qt.io/archive/qt/${QVERSION_SHORT}/${QTVERSION}/single/qt-everywhere-opensource-src-${QTVERSION}.tar.xz 
fi
tar xf qt-everywhere-opensource-src-${QTVERSION}.tar.xz 

cd /qt5/qt-everywhere-opensource-src-$QTV 

./configure -v -skip qtgamepad -platform linux-clang -qt-pcre -qt-xcb -qt-xkbcommon -xkb-config-root /usr/share/X11/xkb -no-pch -qt-sql-sqlite -qt-sql-sqlite2 -confirm-license -opensource 
make -j `nproc`
make install 
rm -Rf /qt5
