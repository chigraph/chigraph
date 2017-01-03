#!/bin/bash

set -e

SCRIPTSDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd $SCRIPTSDIR/..

# generate docs
mkdir -p build
cd build
cmake .. 
cmake --build . --target doc
