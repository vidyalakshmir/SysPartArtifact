#!/bin/sh
dpkg -i build_files/bind/libssl1.1-dbgsym_1.1.1-1ubuntu2.1~18.04.23_amd64.ddeb
tar -xvf nginx-1.17.1.tar.gz
cd nginx-1.17.1/
mkdir nginx-install
CURDIR=$(pwd)/nginx-install
./configure --with-http_ssl_module --prefix=$CURDIR --sbin-path=$CURDIR/sbin/nginx --conf-path=$CURDIR/conf/nginx.conf --error-log-path=$CURDIR/logs/error.log --http-log-path=$CURDIR/logs/access.log --pid-path=$CURDIR/nginx.pid
make && make install
