#!/bin/bash

set -xe


mkdir -p /chigraph.AppDir/
cd /chigraph.AppDir
APPDIR=`pwd`

INSTALL_PREFIX=$APPDIR/usr


export CXX=/opt/llvm/bin/clang++
export CC=/opt/llvm/bin/clang

install_framework() {
  if [ -z "$1" ]; then
    echo "usage: install_framework <framework name>\n"
    return
  fi
  FRAMEWORK="$1"
  
  TMP=`mktemp -d`
  
  wget http://download.kde.org/stable/frameworks/$KF5_VER_SHORT/$FRAMEWORK-$KF5_VER_LONG.tar.xz
  tar xf $FRAMEWORK-$KF5_VER_LONG.tar.xz
  FRAMEWORK_DIR=`pwd`/$FRAMEWORK-$KF5_VER_LONG.tar.xz
  
  mkdir -p $FRAMEWORK_DIR/build
  cd $FRAMWORK_DIR/build
  cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX
  make -j `nproc`
  make install
  
  
  rm -rf $TMP
}

install_framework extra-cmake-modules
install_framework kconfig
install_framework kguiaddons
install_framework ki18n
install_framework kitemviews
install_framework sonnet
install_framework kwindowsystem
install_framework kwidgetsaddons
install_framework kcompletion
install_framework kdbusaddons
install_framework karchive
install_framework kcoreaddons
install_framework kjobwidgets
install_framework kcrash
install_framework kservice
install_framework kcodecs
install_framework kauth
install_framework kconfigwidgets
install_framework kiconthemes
install_framework ktextwidgets
install_framework kglobalaccel
install_framework kxmlgui


cd /
git clone https://github.com/russelltg/chigraph
mkdir -p chigraph/build
cd chigraph/build
cmake .. -DCMAKE_BUILD_TYPE=Release -DINSTALL_PREFIX=$INSTALL_PREFIX
make -j `nproc`
make install
