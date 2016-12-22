#!/bin/bash


SCRIPTSDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd $SCRIPTSDIR/..



clang-format -style=file -i $(find libchig chig gui test -name "*.cpp") $(find test libchig chig gui -name "*.hpp")

