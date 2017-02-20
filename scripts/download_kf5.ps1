$scriptsdir = $PSScriptRoot

$startingdir = $pwd

if ($args.length -ne 1) {
	"Usage: download_kf5.ps1 [Debug|Release]"
	exit
}


$type = $args[0]

$url = ""
if ($type -eq "Debug") {
	$url = "https://github.com/chigraph/chigraph/releases/download/dependencies/kf5-5.30-debug-msvc14-win64.zip"
} else {
	$url = "https://github.com/chigraph/chigraph/releases/download/dependencies/kf5-5.30-release-msvc14-win64.zip"
}

cd $scriptsdir\..\third_party

# download the zip file
$webclient = New-Object System.Net.WebClient

"downloading kf5-$type.zip"
$webclient.DownloadFile("$url", "$pwd/kf5-$type.zip")


Expand-Archive "kf5-$type.zip" .

cd $startingdir