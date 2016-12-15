
$ErrorActionPreference = "Stop"

$firstdir = pwd
$scriptsdir = $PSScriptRoot


$buildtype = $args[0]
$generator = $args[1]
$qtdir = $args[2]

"Building KF5 with $generator in $buildtype mode with qt in $qtdir"

mkdir $scriptsdir/../build/kf5 -Force
mkdir $scriptsdir/../third_party/kf5 -Force

function build_framework
{
	$framework = $args[0]

	cd $scriptsdir/../build/kf5/
	
	if(Test-Path $framework) {
		rm $framework -Recurse -Force
    }
    git clone https://anongit.kde.org/$framework --depth 1
    mkdir $framework/build -Force
    cd $framework/build
       
	"cmake .. -DCMAKE_BUILD_TYPE=`"$buildtype`" -DCMAKE_INSTALL_PREFIX=`"$scriptsdir/../third_party/kf5`" -G`"$generator`" -DCMAKE_PREFIX_PATH=`"$qtdir;$scriptsdir/../third_party/kf5`""

    cmake .. -DCMAKE_BUILD_TYPE="$buildtype" -DCMAKE_INSTALL_PREFIX="$scriptsdir/../third_party/kf5" -G"$generator" -DCMAKE_PREFIX_PATH="$qtdir;$scriptsdir/../third_party/kf5"
	if($LastExitCode) {
		exit
	}
    cmake --build . 
	if($LastExitCode) {
		exit
	}
    cmake --build . --target install
	if($LastExitCode) {
		exit
	}

}

build_framework extra-cmake-modules
build_framework kconfig
build_framework kguiaddons
build_framework ki18n
build_framework kitemviews
build_framework sonnet
build_framework kwidgetsaddons
build_framework kcompletion
build_framework kdbusaddons
build_framework karchive
build_framework kcoreaddons
#build_framework kjobwidgets
build_framework kwindowsystem
build_framework kcrash
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

cd $firstdir