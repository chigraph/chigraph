#!/bin/bash

set -xe


imgdir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

ink=`which inkscape`
$ink -e chigraph.png -d 150 chigraph.svg


