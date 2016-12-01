# Building appimages

Appimages are the preferred installation tecnique for chigraph on linux, as most distros don't have all the newest packages.

This way anyone with a decently new distro can run it flawlessly.

## Build the docker image

The docker image contains LLVM 3.9, , Qt5.7, Kde Frameworks 5.25 (only the bits that chigraph needs)

`docker build .`


