#!/bin/bash

set -e

SCRIPTSDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd $SCRIPTSDIR/appimage

docker build . -t russelltg/chigraph-appimage:latest
docker push russelltg/chigraph-appimage:latest

