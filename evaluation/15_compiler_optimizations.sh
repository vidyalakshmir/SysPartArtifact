#!/bin/bash

cd compiler_optimizations/gcc
BASEDIR=$(pwd)
if [ ! -e "$BASEDIR/binaries/bind/0/bin/named" ]; then
	./make_libuv.sh
	./make_bind.sh
fi
if [ ! -e "$BASEDIR/binaries/httpd/0/bin/httpd" ]; then
	./make_apr.sh
	./make_apr-util.sh 
	./make_httpd.sh 
fi
if [ ! -e "$BASEDIR/binaries/lighttpd/0/sbin/lighttpd" ]; then
	./make_lighttpd.sh
fi 
if [ ! -e "$BASEDIR/binaries/memcached/0/bin/memcached" ]; then
	./make_libevent.sh 
	./make_memcached.sh 
fi
if [ ! -e "$BASEDIR/binaries/nginx/0/sbin/nginx" ]; then
	./make_nginx.sh 
fi
if [ ! -e "$BASEDIR/binaries/redis/0/redis-server" ]; then
	./make_redis.sh
fi

cd ../..
cd compiler_optimizations/clang
BASEDIR=$(pwd)
if [ ! -e "$BASEDIR/binaries/bind/0/bin/named" ]; then
	./make_libuv.sh
	./make_bind.sh
fi
if [ ! -e "$BASEDIR/binaries/httpd/0/bin/httpd" ]; then
	./make_apr.sh
	./make_apr-util.sh 
	./make_httpd.sh 
fi
if [ ! -e "$BASEDIR/binaries/lighttpd/0/sbin/lighttpd" ]; then
	./make_lighttpd.sh
fi 
if [ ! -e "$BASEDIR/binaries/memcached/0/bin/memcached" ]; then
	./make_libevent.sh 
	./make_memcached.sh 
fi
if [ ! -e "$BASEDIR/binaries/nginx/0/sbin/nginx" ]; then
	./make_nginx.sh 
fi
if [ ! -e "$BASEDIR/binaries/redis/0/redis-server" ]; then
	./make_redis.sh
fi

cd ../../
./compiler_opt_static_loop_analysis.h
./compiler_opt_dominant_loop_detection.sh 
./compiler_opt_syscall_filter.sh


