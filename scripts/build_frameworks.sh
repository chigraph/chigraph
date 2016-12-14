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

build_framework extra-cmake-modules > kf5.log || cat kf5.log
build_framework kconfig > kf5.log || cat kf5.log
build_framework kguiaddons > kf5.log || cat kf5.log
build_framework ki18n > kf5.log || cat kf5.log
build_framework kitemviews > kf5.log || cat kf5.log
build_framework sonnet > kf5.log || cat kf5.log
build_framework kwidgetsaddons > kf5.log || cat kf5.log
build_framework kcompletion > kf5.log || cat kf5.log
build_framework kdbusaddons > kf5.log || cat kf5.log
build_framework karchive > kf5.log || cat kf5.log
build_framework kcoreaddons > kf5.log || cat kf5.log
#build_framework kjobwidgets
build_framework kwindowsystem > kf5.log || cat kf5.log
build_framework kcrash > kf5.log || cat kf5.log
build_framework kservice > kf5.log || cat kf5.log
build_framework kcodecs > kf5.log || cat kf5.log
build_framework kauth > kf5.log || cat kf5.log
build_framework kconfigwidgets > kf5.log || cat kf5.log
build_framework kiconthemes > kf5.log || cat kf5.log
build_framework ktextwidgets > kf5.log || cat kf5.log
build_framework kglobalaccel > kf5.log || cat kf5.log
build_framework kxmlgui > kf5.log || cat kf5.log
#build_framework kbookmarks
#build_framework solid
#build_framework kio
#build_framework kparts
#build_framework kitemmodels
#build_framework threadweaver
build_framework attica > kf5.log || cat kf5.log
#build_framework knewstuff
#build_framework ktexteditor
#build_framework kpackage
#build_framework kdeclarative
#build_framework kcmutils
#build_framework knotifications
#build_framework knotifyconfig
#build_framework libkomparediff2
#build_framework kdoctools
build_framework breeze-icons -DBINARY_ICONS_RESOURCE=1 > kf5.log || cat kf5.log
#build_framework kpty
#build_framework kinit 
#build_framework konsole
