#!/bin/bash

CHIGRAPHDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd $CHIGRAPHDIR/

set -xe

function dlAndExtract() {
	url=$1
	filename=$2
	extractto=$3
	
	wget "$url" -O "$filename"
	
	( cd $extractto && tar -xf $filename )
}

tpdir="$CHIGRAPHDIR/third_party"

if [  "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
	
	dlAndExtract 'https://github.com/chigraph/chigraph/releases/download/dependencies/kf5-debug-gcc-linux64.tar.xz' "$tpdir/kf5-debug.tar.xz" $tpdir
	dlAndExtract 'https://github.com/chigraph/chigraph/releases/download/dependencies/kf5-release-gcc-linux64.tar.xz' "$tpdir/kf5-release.tar.xz" $tpdir
	
	
elif [ "$(expr substr $(uname -s) 1 10)" == "MINGW32_NT" ] || [ "$(expr substr $(uname -s) 1 10)" == "MINGW64_NT" ]; then
	dlAndExtract 'https://github.com/chigraph/chigraph/releases/download/dependencies/kf5-5.31.0-release-gcc6.3-win64.tar.xz' "$tpdir/kf5-release.tar.xz" $tpdir
	dlAndExtract 'https://github.com/chigraph/chigraph/releases/download/dependencies/kf5-5.31.0-debug-gcc6.3-win64.tar.xz' "$tpdir/kf5-debug.tar.xz" $tpdir
	
elif [ "$(uname)" == "Darwin" ]; then

	dlAndExtract 'https://github.com/chigraph/chigraph/releases/download/dependencies/kf5-5.31.0-debug-appleclang8-darwin64.tar.xz' "$tpdir/kf5-debug.tar.xz" $tpdir
	dlAndExtract 'https://github.com/chigraph/chigraph/releases/download/dependencies/kf5-5.31.0-release-appleclang8-darwin64.tar.xz' "$tpdir/kf5-release.tar.xz" $tpdir
fi
