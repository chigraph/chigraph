#!bin/bash

set -xe

if [ -z "$LLVM_VER" ]; then
  echo "Must set a LLVM_VER"
  exit
fi

mkdir -p /llvm
cd /llvm

if [ ! -a llvm-$LLVM_VER.src.tar.xz ]; then
  wget http://llvm.org/releases/$LLVM_VER/llvm-$LLVM_VER.src.tar.xz 
fi

tar xvf llvm-$LLVM_VER.src.tar.xz 
cd llvm-$LLVM_VER.src 
cd tools 

# download clang
wget http://llvm.org/releases/$LLVM_VER/cfe-$LLVM_VER.src.tar.xz 
tar xvf cfe-$LLVM_VER.src.tar.xz 

. /opt/rh/python27/enable 
. /opt/rh/devtoolset-4/enable 

python --version 
cd /llvm/llvm-$LLVM_VER.src 
mkdir -p build 
cd build 
cmake3 .. -DCMAKE_INSTALL_PREFIX=/opt/llvm/ -DCMAKE_BUILD_TYPE=Release 
make -j8 install 
rm -Rf /llvm
 
