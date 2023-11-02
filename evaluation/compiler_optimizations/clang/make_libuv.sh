#!/bin/sh
BASEDIR=$(pwd)
mkdir -p libuv/0
mkdir -p libuv/1
mkdir -p libuv/2
mkdir -p libuv/3

cd libuv/0
tar -xvf ../../libuv-1.34.0.tar.gz
cd libuv-1.34.0
export CC=clang  
export AR=llvm-ar 
export RANLIB=llvm-ranlib
export LD=/usr/bin/ld 
export CFLAGS="-flto -O0 -fpic" 
export LDFLAGS="-flto -Wl -pie"
sh autogen.sh 
./configure --prefix=$BASEDIR/binaries/libuv/0/libuv-1.34.0/install
make
make check
make install
cd ../../

cd 1
tar -xvf ../../libuv-1.34.0.tar.gz
cd libuv-1.34.0
export CC=clang  
export AR=llvm-ar 
export LD=/usr/bin/ld 
export CFLAGS="-flto -O1 -fpic" 
export LDFLAGS="-flto -Wl -pie"
sh autogen.sh 
./configure --prefix=$BASEDIR/binaries/libuv/1/libuv-1.34.0/install
make
make check
make install
cd ../../

cd 2
tar -xvf ../../libuv-1.34.0.tar.gz
cd libuv-1.34.0
export CC=clang  
export AR=llvm-ar 
export LD=/usr/bin/ld 
export CFLAGS="-flto -O2 -fpic" 
export LDFLAGS="-flto -Wl -pie"
sh autogen.sh 
./configure --prefix=$BASEDIR/binaries/libuv/2/libuv-1.34.0/install
make
make check
make install
cd ../../

cd 3
tar -xvf ../../libuv-1.34.0.tar.gz
cd libuv-1.34.0
export CC=clang  
export AR=llvm-ar 
export LD=/usr/bin/ld 
export CFLAGS="-flto -O3 -fpic" 
export LDFLAGS="-flto -Wl -pie"
sh autogen.sh 
./configure --prefix=$BASEDIR/binaries/libuv/3/libuv-1.34.0/install
make
make check
make install
cd ../../..

