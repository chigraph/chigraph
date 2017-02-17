
$firstdir = pwd
$scriptsdir = $PSScriptRoot

$version = "5.31.0"
$sversion = "5.31"

$qtdir = $args[0]
$buildtype = $args[1]
$cmakeargs = $args[2..($args.length - 1)]

"Building KF5 with in $buildtype mode with cmake arguments $cmakeargs and qt in $qtdir"

# set path
$env:Path = "$qtdir/bin;$env:Path"

$kf5dir = "$scriptsdir/../third_party/kf5"
mkdir $kf5dir -Force
mkdir $kf5dir/build -Force

$webclient = New-Object System.Net.WebClient



function build_framework
{
	$framework = $args[0]

	cd $scriptsdir/../third_party/kf5/build
	
	$foldername = "$framework-$version"
	
	$url = "http://download.kde.org/stable/frameworks/$sversion/$foldername.zip"
		
	if (!(Test-Path "$foldername.zip")) {
		"Downloading $framework"
		$webclient.DownloadFile("$url", "$pwd/$foldername.zip")
	}
	if (!(Test-Path -Path $foldername)) {
		"Extracting $framework"
		Expand-Archive "$foldername.zip" .
		
	}
	
    mkdir $foldername/build -Force
    cd $foldername/build
       
	$gtdir = "$scriptsdir/../third_party/libintl-win32"
	$zlibdir = "$scriptsdir/../third_party/zlib-win32"
	$fbdir = "$scriptsdir/../third_party/flexbison-win32"
	
	cmake .. -G"Visual Studio 14 2015 Win64" -DCMAKE_BUILD_TYPE=$buildtype -DCMAKE_INSTALL_PREFIX="$kf5dir" -DCMAKE_PREFIX_PATH="$kf5dir;$qtdir" `
		-DGETTEXT_MSGMERGE_EXECUTABLE="$gtdir/bin/msgmerge.exe" -DGETTEXT_MSGFMT_EXECUTABLE="$gtdir/bin/msgfmt.exe" `
		-DLibIntl_INCLUDE_DIRS="$gtdir/include" -DLibIntl_LIBRARIES="$gtdir/lib/libintl.lib" -DZLIB_LIBRARY="$zlibdir/lib/zlibstatic.lib" `
		-DZLIB_INCLUDE_DIR="$zlibdir/include" -DFLEX_EXECUTABLE="$fbdir/bin/flex.exe" -DBISON_EXECUTABLE="$fbdir/bin/bison.exe" `
		-DCMAKE_CXX_FLAGS="-DKIOWIDGETS_NO_DEPRECATED" `
		"$cmakeargs" 

	if($LastExitCode) {
		#exit
	}
    cmake --build . --target install --config $buildtype
	if($LastExitCode) {
		exit
	}

}

build_framework extra-cmake-modules
build_framework kconfig
build_framework ki18n
build_framework kguiaddons
build_framework kitemviews
build_framework sonnet
build_framework kwidgetsaddons
build_framework kcompletion
build_framework kdbusaddons
build_framework karchive
build_framework kcoreaddons
build_framework kjobwidgets
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
build_framework kbookmarks
build_framework solid
#build_framework kwallet
build_framework kio
build_framework kparts
#build_framework kitemmodels
#build_framework threadweaver
build_framework attica
#build_framework knewstuff
build_framework syntax-highlighting
build_framework ktexteditor
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
