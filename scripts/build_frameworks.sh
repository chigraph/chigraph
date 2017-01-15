#/bin/bash

set -xe

SCRIPTSDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd $SCRIPTSDIR/..

flags=$@

build_framework() {
    framework=$1
    
    cd /tmp/
    rm -rf $framework
    
    git clone https://anongit.kde.org/$framework --depth 1
    mkdir $framework/build
    cd $framework/build
    
    cmake .. -DCMAKE_PREFIX_PATH=$SCRIPTSDIR/../third_party/kf5 -DCMAKE_INSTALL_PREFIX=$SCRIPTSDIR/../third_party/kf5 $flags 
    cmake --build . -- 
    cmake --build . --target install
}
cd /tmp

build_helper() {
    ( (build_framework $1)  &> kf5.log) || cat kf5.log
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
#build_helper kjobwidgets
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
#build_helper kbookmarks
#build_helper solid
#build_helper kio
#build_helper kparts
#build_helper kitemmodels
#build_helper threadweaver
build_helper attica
#build_helper knewstuff
#build_helper ktexteditor
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
