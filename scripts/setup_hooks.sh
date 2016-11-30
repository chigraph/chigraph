#!/bin/bash

set -xe

SCRIPTSDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd $SCRIPTSDIR/..

# see if clang-format is in $PATH -- this will fail if it isn't in path
FMT=$(which clang-format)

cp scripts/hooks/pre-commit .git/hooks/
