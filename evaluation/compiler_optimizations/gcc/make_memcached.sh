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

tar -xvf memcached.tar.gz
cd memcached
export CFLAGS="-O0"
./configure --prefix=$BASEDIR/binaries/memcached/0 --with-libevent=$BASEDIR/binaries/libevent/0 --enable-sasl
make
make install
cd ..
rm -rf memcached

tar -xvf memcached.tar.gz
cd memcached
export CFLAGS="-O1"
./configure --prefix=$BASEDIR/binaries/memcached/1 --with-libevent=$BASEDIR/binaries/libevent/1 --enable-sasl
make
make install
cd ..
rm -rf memcached

tar -xvf memcached.tar.gz
cd memcached
export CFLAGS="-O2"
./configure --prefix=$BASEDIR/binaries/memcached/2 --with-libevent=$BASEDIR/binaries/libevent/2 --enable-sasl
make
make install
cd ..
rm -rf memcached

tar -xvf memcached.tar.gz
cd memcached
export CFLAGS="-O3"
./configure --prefix=$BASEDIR/binaries/memcached/3 --with-libevent=$BASEDIR/binaries/libevent/3 --enable-sasl
make
make install
cd ..
rm -rf memcached
