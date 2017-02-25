#!/bin/sh


pacman -Syu --noconfirm

pacman -S --noconfirm mingw64-x86_64-toolchain mingw64-x86_64-llvm mingw64-x86_64-clang mingw-w64-x86_64-cmake mingw-w64-x86_64-ninja mingw-w64-x86_64-qt5 git bison flex mingw-w64-x86_64-python3 

