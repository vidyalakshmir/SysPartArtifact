#!/bin/bash	
BASEDIR=$(pwd)
EVALDIR=$(dirname $BASEDIR)
cd ../analysis/app
echo "Building the tools"
cp ../tools/src_files/elfdynamic.cpp ../tools/egalito/src/elf/elfdynamic.cpp
make
for compiler in gcc clang; do
	for i in 0 1 2 3; do
		export BIND_OPT=$BASEDIR/compiler_optimizations/$compiler/binaries/bind/$i/sbin/named
		export HTTPD_OPT=$BASEDIR/compiler_optimizations/$compiler/binaries/httpd/$i/bin/httpd
		export LIGHTTPD_OPT=$BASEDIR/compiler_optimizations/$compiler/binaries/lighttpd/$i/sbin/lighttpd
		export MEMCACHED_OPT=$BASEDIR/compiler_optimizations/$compiler/binaries/memcached/$i/bin/memcached
		export NGINX_OPT=$BASEDIR/compiler_optimizations/$compiler/binaries/nginx/$i/sbin/nginx
		export REDIS_OPT=$BASEDIR/compiler_optimizations/$compiler/binaries/redis/$i/redis-server
		
		export BIND_OUT=$EVALDIR/outputs/bind/opt/$compiler/$i
		export HTTPD_OUT=$EVALDIR/outputs/httpd/opt/$compiler/$i
		export LIGHTTPD_OUT=$EVALDIR/outputs/lighttpd/opt/$compiler/$i
		export MEMCACHED_OUT=$EVALDIR/outputs/memcached/opt/$compiler/$i
		export NGINX_OUT=$EVALDIR/outputs/nginx/opt/$compiler/$i
		export REDIS_OUT=$EVALDIR/outputs/redis/opt/$compiler/$i
		
		mkdir -p $BIND_OUT
		mkdir -p $HTTPD_OUT
		mkdir -p $LIGHTTPD_OUT
		mkdir -p $MEMCACHED_OUT
		mkdir -p $NGINX_OUT
		mkdir -p $REDIS_OUT
			
		echo "Running static loop analysis on $BIND_OPT"
		./loops $BIND_OPT > $BIND_OUT/temp.out
		grep '^FUNC' $BIND_OUT/temp.out > $BIND_OUT/functions.out
		grep -v '^FUNC' $BIND_OUT/temp.out > $BIND_OUT/loops.out
		echo "Output written to $BIND_OUT/loops.out"
		echo "Running static loop analysis on $HTTPD_OPT"

		./loops $HTTPD_OPT > $HTTPD_OUT/temp.out
		grep '^FUNC' $HTTPD_OUT/temp.out > $HTTPD_OUT/functions.out
		grep -v '^FUNC' $HTTPD_OUT/temp.out > $HTTPD_OUT/loops.out
		echo "Output written to $HTTPD_OUT/loops.out"
		
		echo "Running static loop analysis on $LIGHTTPD_OPT"

		./loops $LIGHTTPD_OPT > $LIGHTTPD_OUT/temp.out
		grep '^FUNC' $LIGHTTPD_OUT/temp.out > $LIGHTTPD_OUT/functions.out
		grep -v '^FUNC' $LIGHTTPD_OUT/temp.out > $LIGHTTPD_OUT/loops.out
		echo "Output written to $LIGHTTPD_OUT/loops.out"
		
		echo "Running static loop analysis on $MEMCACHED_OPT"
		./loops $MEMCACHED_OPT > $MEMCACHED_OUT/temp.out
		grep '^FUNC' $MEMCACHED_OUT/temp.out > $MEMCACHED_OUT/functions.out
		grep -v '^FUNC' $MEMCACHED_OUT/temp.out > $MEMCACHED_OUT/loops.out
		echo "Output written to $MEMCACHED_OUT/loops.out"
		
		echo "Running static loop analysis on $REDIS_OPT"
		./loops $REDIS_OPT > $REDIS_OUT/temp.out
		grep '^FUNC' $REDIS_OUT/temp.out > $REDIS_OUT/functions.out
		grep -v '^FUNC' $REDIS_OUT/temp.out > $REDIS_OUT/loops.out
		echo "Output written to $REDIS_OUT/loops.out"
		
		echo "Running static loop analysis on $NGINX_OPT"
		./loops $NGINX_OPT > $NGINX_OUT/temp.out
		grep '^FUNC' $NGINX_OUT/temp.out > $NGINX_OUT/functions.out
		grep -v '^FUNC' $NGINX_OUT/temp.out > $NGINX_OUT/loops.out
		echo "Output written to $NGINX_OUT/loops.out"
	done
done	
echo "STATIC LOOP ANALYSIS FINISHED"
cd $EVALDIR