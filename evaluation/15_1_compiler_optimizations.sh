#!/bin/bash

BASEDIR=$(pwd)/compiler_optimizations/gcc
if [ ! -e "$BASEDIR/binaries/bind/0/bin/named" ]; then
	./compiler_opt_create_binaries.sh gcc libuv
	./compiler_opt_create_binaries.sh gcc bind
fi
if [ ! -e "$BASEDIR/binaries/httpd/0/bin/httpd" ]; then
	./compiler_opt_create_binaries.sh gcc apr
	./compiler_opt_create_binaries.sh gcc apr-util
	./compiler_opt_create_binaries.sh gcc httpd

fi
if [ ! -e "$BASEDIR/binaries/lighttpd/0/sbin/lighttpd" ]; then
	./compiler_opt_create_binaries.sh gcc lighttpd
fi 
if [ ! -e "$BASEDIR/binaries/memcached/0/bin/memcached" ]; then
	./compiler_opt_create_binaries.sh gcc libevent
	/compiler_opt_create_binaries.sh gcc memcached
fi
if [ ! -e "$BASEDIR/binaries/nginx/0/sbin/nginx" ]; then
	./compiler_opt_create_binaries.sh gcc nginx
fi
if [ ! -e "$BASEDIR/binaries/redis/0/redis-server" ]; then
	./compiler_opt_create_binaries.sh gcc redis
fi

BASEDIR=$(pwd)/compiler_optimizations/clang
if [ ! -e "$BASEDIR/binaries/bind/0/bin/named" ]; then
        ./compiler_opt_create_binaries.sh clang libuv
        ./compiler_opt_create_binaries.sh clang bind
fi
if [ ! -e "$BASEDIR/binaries/httpd/0/bin/httpd" ]; then
        ./compiler_opt_create_binaries.sh clang apr
        ./compiler_opt_create_binaries.sh clang apr-util
        ./compiler_opt_create_binaries.sh clang httpd

fi
if [ ! -e "$BASEDIR/binaries/lighttpd/0/sbin/lighttpd" ]; then
        ./compiler_opt_create_binaries.sh clang lighttpd
fi
if [ ! -e "$BASEDIR/binaries/memcached/0/bin/memcached" ]; then
        ./compiler_opt_create_binaries.sh clang libevent
        /compiler_opt_create_binaries.sh clang memcached
fi
if [ ! -e "$BASEDIR/binaries/nginx/0/sbin/nginx" ]; then
        ./compiler_opt_create_binaries.sh clang nginx
fi
if [ ! -e "$BASEDIR/binaries/redis/0/redis-server" ]; then
        ./compiler_opt_create_binaries.sh clang redis
fi
