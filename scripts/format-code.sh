#!/bin/bash


SCRIPTSDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd $SCRIPTSDIR/..



clang-format -style=file -i $(find libchig chig -name "*.cpp") $(find libchig chig -name "*.hpp")

