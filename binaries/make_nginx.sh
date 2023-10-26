#!/bin/sh
tar -xvf nginx-1.17.1.tar.gz
cd nginx-1.17.1/
mkdir nginx-install
CURDIR=$(pwd)/nginx-install
./configure --with-http_ssl_module --prefix=$CURDIR --sbin-path=$CURDIR/sbin/nginx --conf-path=$CURDIR/conf/nginx.conf --error-log-path=$CURDIR/logs/error.log --http-log-path=$CURDIR/logs/access.log --pid-path=$CURDIR/nginx.pid
make && make install
