#!/bin/bash


SCRIPTSDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd $SCRIPTSDIR/..



clang-format -style=file -i $(find chig chigc -name "*.cpp") $(find chig chigc -name "*.hpp")

