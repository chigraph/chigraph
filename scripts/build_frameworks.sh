#/bin/bash

set -xe

SCRIPTSDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd $SCRIPTSDIR/..

kf5dir=$SCRIPTSDIR/../third_party/kf5
mkdir -p $kf5dir/build

version=5.31.0
sversion=${version:0:4}

flags=$@

build_framework() {
    framework=$1
    
    cd $kf5dir/build

    foldername=$framework-$version

    wget http://download.kde.org/stable/frameworks/$sversion/$foldername.tar.xz
    tar xf $foldername.tar.xz
    mkdir -p $foldername/build
    cd $foldername/build
    
    cmake .. -DCMAKE_PREFIX_PATH=$kf5dir -DCMAKE_INSTALL_PREFIX=$kf5dir $flags 
    cmake --build .
    cmake --build . --target install
}

build_helper() {
    ( (build_framework $1)  &> kf5.log) || (cat kf5.log && exits)
}

build_helper extra-cmake-modules
build_helper kconfig
build_helper kguiaddons
build_helper ki18n
build_helper kitemviews
build_helper sonnet
build_helper kwidgetsaddons
build_helper kcompletion
build_helper kdbusaddons
build_helper karchive
build_helper kcoreaddons
build_helper kjobwidgets
build_helper kwindowsystem
build_helper kcrash
build_helper kservice
build_helper kcodecs
build_helper kauth
build_helper kconfigwidgets
build_helper kiconthemes
build_helper ktextwidgets
build_helper kglobalaccel
build_helper kxmlgui
build_helper kbookmarks
build_helper solid
build_helper kio
#build_helper kparts
#build_helper kitemmodels
#build_helper threadweaver
build_helper attica
#build_helper knewstuff
build_helper ktexteditor
#build_helper kpackage
#build_helper kdeclarative
#build_helper kcmutils
#build_helper knotifications
#build_helper knotifyconfig
#build_helper libkomparediff2
#build_helper kdoctools
build_helper breeze-icons -DBINARY_ICONS_RESOURCE=1
#build_helper kpty
#build_helper kinit 
#build_helper konsole
