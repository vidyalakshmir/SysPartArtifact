#!/bin/sh
BASEDIR=$(pwd)
mkdir -p libuv/0
mkdir -p libuv/1
mkdir -p libuv/2
mkdir -p libuv/3

cd libuv/0
tar -xvf ../../../zips/libuv-1.34.0.tar.gz
cd libuv-1.34.0
export CFLAGS="-O0"
sh autogen.sh 
./configure --prefix=$BASEDIR/binaries/libuv/0/libuv-1.34.0/install
make
make check
make install
cd ../../

cd 1
tar -xvf ../../../zips/libuv-1.34.0.tar.gz
cd libuv-1.34.0
export CFLAGS="-O1"
sh autogen.sh 
./configure --prefix=$BASEDIR/binaries/libuv/1/libuv-1.34.0/install
make
make check
make install
cd ../../

cd 2
tar -xvf ../../../zips/libuv-1.34.0.tar.gz
cd libuv-1.34.0
export CFLAGS="-O2"
sh autogen.sh 
./configure --prefix=$BASEDIR/binaries/libuv/2/libuv-1.34.0/install
make
make check
make install
cd ../../

cd 3
tar -xvf ../../../zips/libuv-1.34.0.tar.gz
cd libuv-1.34.0
export CFLAGS="-O3"
sh autogen.sh 
./configure --prefix=$BASEDIR/binaries/libuv/3/libuv-1.34.0/install
make
make check
make install
cd ../../..

