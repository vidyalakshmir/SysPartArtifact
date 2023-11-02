#!/bin/bash

cd compiler_optimizations
cd gcc 
./make_libuv.sh
./make_bind.sh
./make_apr.sh
./make_apr-util.sh 
./make_httpd.sh 
./make_lighttpd.sh 
./make_libevent.sh 
./make_memcached.sh 
./make_nginx.sh 
./make_redis.sh

cd compiler_optimizations
cd clang 
./make_libuv.sh
./make_bind.sh
./make_apr.sh
./make_apr-util.sh 
./make_httpd.sh 
./make_lighttpd.sh 
./make_libevent.sh 
./make_memcached.sh 
./make_nginx.sh 
./make_redis.sh
