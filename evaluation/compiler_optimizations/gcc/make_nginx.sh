#!/bin/bash

BASEDIR=$(pwd)
mkdir -p binaries/nginx/0
mkdir -p binaries/nginx/1
mkdir -p binaries/nginx/2
mkdir -p binaries/nginx/3
mkdir -p output/nginx/0/pin
mkdir -p output/nginx/1/pin
mkdir -p output/nginx/2/pin
mkdir -p output/nginx/3/pin
tar -xvf ../zips/nginx-1.17.1.tar.gz
cd nginx-1.17.1/
export CFLAGS="-O0"
CURDIR=$BASEDIR/binaries/nginx/0
mkdir -p $CURDIR

./configure --with-http_ssl_module --prefix=$CURDIR --sbin-path=$CURDIR/sbin/nginx --conf-path=$CURDIR/conf/nginx.conf --error-log-path=$CURDIR/logs/error.log --http-log-path=$CURDIR/logs/access.log --pid-path=$CURDIR/nginx.pid

make && make install

cd ../

rm -rf nginx-1.17.1/

tar -xvf ../zips/nginx-1.17.1.tar.gz

cd nginx-1.17.1/

export CFLAGS="-O1"
CURDIR=$BASEDIR/binaries/nginx/1
mkdir -p $CURDIR

./configure --with-http_ssl_module --prefix=$CURDIR --sbin-path=$CURDIR/sbin/nginx --conf-path=$CURDIR/conf/nginx.conf --error-log-path=$CURDIR/logs/error.log --http-log-path=$CURDIR/logs/access.log --pid-path=$CURDIR/nginx.pid

make && make install

cd ../

rm -rf nginx-1.17.1/

tar -xvf ../zips/nginx-1.17.1.tar.gz

cd nginx-1.17.1/

export CFLAGS="-O2"

CURDIR=$BASEDIR/binaries/nginx/2
mkdir -p $CURDIR

./configure --with-http_ssl_module --prefix=$CURDIR --sbin-path=$CURDIR/sbin/nginx --conf-path=$CURDIR/conf/nginx.conf --error-log-path=$CURDIR/logs/error.log --http-log-path=$CURDIR/logs/access.log --pid-path=$CURDIR/nginx.pid

make && make install

cd ../

rm -rf nginx-1.17.1/

tar -xvf ../zips/nginx-1.17.1.tar.gz

cd nginx-1.17.1/

export CFLAGS="-O3"

CURDIR=$BASEDIR/binaries/nginx/3
mkdir -p $CURDIR

./configure --with-http_ssl_module --prefix=$CURDIR --sbin-path=$CURDIR/sbin/nginx --conf-path=$CURDIR/conf/nginx.conf --error-log-path=$CURDIR/logs/error.log --http-log-path=$CURDIR/logs/access.log --pid-path=$CURDIR/nginx.pid

make && make install


