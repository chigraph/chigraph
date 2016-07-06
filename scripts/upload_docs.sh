#!/bin/bash

set -e

SCRIPTSDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd ..

# generate docs
mkdir -p build
cd build
cmake .. 
make doc
cd ..

# make a temporary dir to store them

TMP=$(mktemp -d)

# store the files
cp build/doc/html/* $TMPs

# go the the gh-pages branch and add them
git checkout gh-pages

# delete everything that is here
rm $(find . -maxdepth 1  -not -path "./.git" -not -path '.')

# copy it in
cp $TMP/* ./

# commit
git add .
git commit -m"[BOT] Update docs"
