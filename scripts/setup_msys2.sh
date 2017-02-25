#!/bin/sh


pacman -Syu --noconfirm

pacman -S --noconfirm mingw-w64-x86_64-toolchain mingw-w64-x86_64-llvm mingw-w64-x86_64-clang mingw-w64-x86_64-clang-tools-extra mingw-w64-x86_64-cmake mingw-w64-x86_64-ninja mingw-w64-x86_64-qt5 git bison flex mingw-w64-x86_64-python3 

