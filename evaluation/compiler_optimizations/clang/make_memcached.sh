#!/bin/sh
BASEDIR=$(pwd)
mkdir -p binaries/memcached/0
mkdir -p binaries/memcached/1
mkdir -p binaries/memcached/2
mkdir -p binaries/memcached/3
mkdir -p output/memcached/0/pin
mkdir -p output/memcached/1/pin
mkdir -p output/memcached/2/pin
mkdir -p output/memcached/3/pin

tar -xvf ../zips/memcached.tar.gz
cd memcached
export CC=clang  
export AR=llvm-ar 
export RANLIB=llvm-ranlib
export LD=/usr/bin/ld 
export CFLAGS="-flto -O0 -fpic" 
export LDFLAGS="-flto -Wl -pie"
./configure --prefix=$BASEDIR/binaries/memcached/0 --with-libevent=$BASEDIR/binaries/libevent/0 --enable-sasl
make -j 8
make install
cd ..
rm -rf memcached

tar -xvf ../zips/memcached.tar.gz
cd memcached
export CC=clang  
export AR=llvm-ar 
export LD=/usr/bin/ld 
export CFLAGS="-flto -O1 -fpic" 
export LDFLAGS="-flto -Wl -pie"
./configure --prefix=$BASEDIR/binaries/memcached/1 --with-libevent=$BASEDIR/binaries/libevent/1 --enable-sasl
make -j 8
make install
cd ..
rm -rf memcached

tar -xvf ../zips/memcached.tar.gz
cd memcached
export CC=clang  
export AR=llvm-ar 
export LD=/usr/bin/ld 
export CFLAGS="-flto -O2 -fpic" 
export LDFLAGS="-flto -Wl -pie"
./configure --prefix=$BASEDIR/binaries/memcached/2 --with-libevent=$BASEDIR/binaries/libevent/2 --enable-sasl
make -j 8
make install
cd ..
rm -rf memcached

tar -xvf ../zips/memcached.tar.gz
cd memcached
export CC=clang  
export AR=llvm-ar 
export LD=/usr/bin/ld 
export CFLAGS="-flto -O3 -fpic" 
export LDFLAGS="-flto -Wl -pie"
./configure --prefix=$BASEDIR/binaries/memcached/3 --with-libevent=$BASEDIR/binaries/libevent/3 --enable-sasl
make -j 8
make install
cd ..
rm -rf memcached
