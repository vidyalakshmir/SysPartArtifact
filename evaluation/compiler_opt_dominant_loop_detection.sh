#!/bin/bash	
BASEDIR=$(pwd)
EVALDIR=$(dirname $BASEDIR)
export PIN_ROOT="$BASEDIR/analysis/tools/pin-3.11-97998-g7ecce2dac-gcc-linux"

cd ../analysis/app/src/pintool
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




		echo ""
		echo "Running dominant loop analysis on $BIND_OPT"
		mkdir -p $BIND_OUT/pin
		rm -rf $BIND_OUT/pin/*
		rm -f $BIND_OUT/pin.out
		echo "Press Ctrl+C after 10 seconds to exit"
		sudo $PIN_ROOT/pin -follow_execv -t obj-intel64/timeofouterloop.so -i $BIND_OUT/loops.out -p $BIND_OUT/pin/ -o pin.out -- $BIND -f -u bind
		../scripts/parse_pinout.sh $BIND_OUT/pin
		echo ""
		echo ""
		echo "The main loops of bind compiled with $compiler and optimization O$i are : "
		cat $BIND_OUT/pin.out
		echo ""


		echo ""
		echo "Running dominant loop analysis on $HTTPD_OPT"
		mkdir -p $HTTPD_OUT/pin
		rm -rf $HTTPD_OUT/pin/*
		rm -f $HTTPD_OUT/pin.out
		sudo $PIN_ROOT/pin -follow_execv -t obj-intel64/timeofouterloop.so -i $HTTPD_OUT/loops.out -p $HTTPD_OUT/pin/ -o pin.out -- $HTTPD
		sleep 5
		sudo $HTTPD -k graceful-stop
		sleep 5
		../scripts/parse_pinout.sh $HTTPD_OUT/pin
		echo ""
		echo ""
		echo "The main loops of httpd compiled with $compiler and optimization O$i are : "
		cat $HTTPD_OUT/pin.out
		echo ""

		echo ""
		echo "Running dominant loop analysis on $LIGHTTPD_OUT"
		mkdir -p $LIGHTTPD_OUT/pin
		rm -rf $LIGHTTPD_OUT/pin/*
		rm -f $LIGHTTPD_OUT/pin.out
		echo "Press Ctrl+C after 10 seconds to exit"
		$PIN_ROOT/pin -follow_execv -t obj-intel64/timeofouterloop.so -i $LIGHTTPD_OUT/loops.out -p $LIGHTTPD_OUT/pin/ -o pin.out -- $LIGHTTPD -D -f /home/syspart/SysPartArtifact/binaries/lighttpd-1.4.54/lighttpd-install/config/lighttpd.conf
		../scripts/parse_pinout.sh $LIGHTTPD_OUT/pin
		echo ""
		echo ""
		echo "The main loops of lighttpd compiled with $compiler and optimization O$i are : "
		cat $LIGHTTPD_OUT/pin.out
		echo ""

		echo ""
		echo "Running dominant loop analysis on $MEMCACHED_OPT"
		mkdir -p $MEMCACHED_OUT/pin
		rm -rf $MEMCACHED_OUT/pin/*
		rm -f $MEMCACHED_OUT/pin.out
		echo "Press Ctrl+C after 10 seconds to exit"
		$PIN_ROOT/pin -follow_execv -t obj-intel64/timeofouterloop.so -i $MEMCACHED_OUT/loops.out -p $MEMCACHED_OUT/pin/ -o pin.out -- $MEMCACHED -S -m 64 -p 11211 -u memcache -l 127.0.0.1
		../scripts/parse_pinout.sh $MEMCACHED_OUT/pin
		echo ""
		echo ""
		echo "The main loops of memcached compiled with $compiler and optimization O$i are : "
		cat $MEMCACHED_OUT/pin.out

		echo ""
		echo "Running dominant loop analysis on $NGINX_OPT"
		mkdir -p $NGINX_OUT/pin
		rm -rf $NGINX_OUT/pin/*
		rm -f $NGINX_OUT/pin.out
		sudo $PIN_ROOT/pin -follow_execv -t obj-intel64/timeofouterloop.so -i $NGINX_OUT/loops.out -p $NGINX_OUT/pin/ -o pin.out -- $NGINX
		sleep 5
		sudo $NGINX -s quit
		sleep 5
		../scripts/parse_pinout.sh $NGINX_OUT/pin
		echo ""
		echo ""
		echo "The main loops of nginx compiled with $compiler and optimization O$i are : "
		cat $NGINX_OUT/pin.out

		echo ""
		echo "Running dominant loop analysis on $REDIS_OPT"
		echo "Press Ctrl+C after 10 seconds to exit"
		sleep 2
		mkdir -p $REDIS_OUT/pin
		rm -rf $REDIS_OUT/pin/*
		rm -f $REDIS_OUT/pin.out
		sudo $PIN_ROOT/pin -follow_execv -t obj-intel64/timeofouterloop.so -i $REDIS_OUT/loops.out -p $REDIS_OUT/pin/ -o pin.out -- $REDIS
		sleep 5
		../scripts/parse_pinout.sh $REDIS_OUT/pin
		echo ""
		echo ""
		echo "The main loops of redis compiled with $compiler and optimization O$i are : "
		cat $REDIS_OUT/pin.out
	done
done

cd $EVALDIR