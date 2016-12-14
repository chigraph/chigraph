#/bin/bash

set -xe

SCRIPTSDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd $SCRIPTSDIR/..


buildtype=$1
generator=$2

build_framework() {
    framework=$1
    
    cd /tmp/
    rm -rf $framework
    
    git clone https://anongit.kde.org/$framework --depth 1
    mkdir $framework/build
    cd $framework/build
    
    cmake .. -DCMAKE_BUILD_TYPE=$buildtype -DCMAKE_INSTALL_PREFIX=$SCRIPTSDIR/../third_party/kf5 -G"$generator"
    cmake --build . 
    cmake --build . --target install
}

build_framework extra-cmake-modules
build_framework kconfig
build_framework kguiaddons
build_framework ki18n
build_framework kitemviews
build_framework sonnet
#build_framework kwindowsystem
build_framework kwidgetsaddons
#build_framework kcompletion
build_framework kdbusaddons
build_framework karchive
build_framework kcoreaddons
#build_framework kjobwidgets
#build_framework kcrash
build_framework kservice
build_framework kcodecs
build_framework kauth
build_framework kconfigwidgets
build_framework kiconthemes
build_framework ktextwidgets
build_framework kglobalaccel
build_framework kxmlgui
#build_framework kbookmarks
#build_framework solid
#build_framework kio
#build_framework kparts
#build_framework kitemmodels
#build_framework threadweaver
build_framework attica
#build_framework knewstuff
#build_framework ktexteditor
#build_framework kpackage
#build_framework kdeclarative
#build_framework kcmutils
#build_framework knotifications
#build_framework knotifyconfig
#build_framework libkomparediff2
#build_framework kdoctools
build_framework breeze-icons -DBINARY_ICONS_RESOURCE=1
#build_framework kpty
#build_framework kinit 
#build_framework konsole
