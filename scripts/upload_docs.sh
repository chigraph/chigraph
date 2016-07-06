#!/bin/bash

set -e

SCRIPTSDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd $SCRIPTSDIR/..

# generate docs
mkdir -p build
cd build
cmake .. 
make doc
cd ..

# make a temporary dir to store them

TMP=$(mktemp -d)

# store the files
rsync build/doc/html/* $TMP/

# go the the gh-pages branch and add them
git checkout gh-pages

# delete everything that is here
rm -r $(find . -maxdepth 1  -not -path "./.git" -not -path '.')

# copy it in
rsync $TMP/* ./

rm -rf $TMP

# commit
git add .

# if there is an error, then there is nothing to do, that's fine
set +e
git commit -m"[BOT] Update docs"
set -e

git push
git checkout master
