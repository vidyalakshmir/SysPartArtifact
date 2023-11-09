#!/bin/bash	
BASEDIR=$(pwd)
EVALDIR=$(dirname $BASEDIR)

cd ../analysis/app
cp ../tools/src_files/elfdynamic_libc_2.24.cpp ../tools/egalito/src/elf/elfdynamic.cpp
cp ../tools/src_files/nss_libc_2.24.cpp src/nss.cpp
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

		echo "GENERATING SYSCALL FILTER FOR $BIND_OPT "
		addr=$(grep -w 'main' $BIND_OUT/pin.out | awk {'print $2'} | sort | uniq)
		./syspart -p $BIND_OPT -i -s main -a 2,$addr,main > $BIND_OUT/syscalls.out
		grep -w 'MAINLOOP' $BIND_OUT/syscalls.out | awk {'print $2'} > $BIND_OUT/mainloop_syscalls.out
		echo -n "#Syscalls of main loop of bind compiled with $compiler and optimization O$i is : "
		cat $BIND_OUT/mainloop_syscalls.out


		echo "GENERATING SYSCALL FILTER FOR $HTTPD_OPT "
		addr=$(grep -w 'child_main' $HTTPD_OUT/pin.out | awk {'print $2'} | sort | uniq)
		./syspart -p $HTTPD_OPT -i -s main -a 2,$addr,child_main > $HTTPD_OUT/syscalls.out
		grep -w 'MAINLOOP' $HTTPD_OUT/syscalls.out | awk {'print $2'} > $HTTPD_OUT/mainloop_syscalls.out
		echo -n "#Syscalls of main loop of httpd compiled with $compiler and optimization O$i is : "
		cat $HTTPD_OUT/mainloop_syscalls.out

		echo "GENERATING SYSCALL FILTER FOR $LIGHTTPD_OPT "
		addr=$(grep -w 'main' $LIGHTTPD_OUT/pin.out | awk {'print $2'} | sort | uniq)
		./syspart -p $LIGHTTPD_OPT -i -s main -a 2,$addr,main > $LIGHTTPD_OUT/syscalls.out
		grep -w 'MAINLOOP' $LIGHTTPD_OUT/syscalls.out | awk {'print $2'} > $LIGHTTPD_OUT/mainloop_syscalls.out
		echo -n "#Syscalls of main loop of lighttpd compiled with $compiler and optimization O$i is : "
		cat $LIGHTTPD_OUT/mainloop_syscalls.out

		echo "GENERATING SYSCALL FILTER FOR $MEMCACHED_OPT "
		addr=$(grep -w 'event_base_loop' $MEMCACHED_OUT/pin.out | awk {'print $2'} | sort | uniq)
		./syspart -p $MEMCACHED_OPT -i -s main -a 2,$addr,event_base_loop > $MEMCACHED_OUT/syscalls.out
		grep -w 'MAINLOOP' $MEMCACHED_OUT/syscalls.out | awk {'print $2'} > $MEMCACHED_OUT/mainloop_syscalls.out
		echo -n "#Syscalls of main loop of memcached compiled with $compiler and optimization O$i is : "
		cat $MEMCACHED_OUT/mainloop_syscalls.out

		echo "GENERATING SYSCALL FILTER FOR $NGINX_OPT "
		addr=$(grep -w 'ngx_worker_process_cycle' $NGINX_OUT/pin.out | awk {'print $2'} | sort | uniq)
		./syspart -p $NGINX_OPT -i -s main -a 2,$addr,ngx_worker_process_cycle > $NGINX_OUT/syscalls.out
		grep -w 'MAINLOOP' $NGINX_OUT/syscalls.out | awk {'print $2'} > $NGINX_OUT/mainloop_syscalls.out
		echo -n "#Syscalls of main loop of nginx compiled with $compiler and optimization O$i is : "
		cat $NGINX_OUT/mainloop_syscalls.out

		echo "GENERATING SYSCALL FILTER FOR $REDIS_OPT "
		addr=$(grep -w 'aeMain' $REDIS_OUT/pin.out | awk {'print $2'} | sort | uniq)
		./syspart -p $REDIS_OPT -i -s main -a 2,$addr,aeMain > $REDIS_OUT/syscalls.out
		grep -w 'MAINLOOP' $REDIS_OUT/syscalls.out | awk {'print $2'} > $REDIS_OUT/mainloop_syscalls.out
		echo -n "#Syscalls of main loop of redis compiled with $compiler and optimization O$i is : "
		cat $REDIS_OUT/mainloop_syscalls.out
	done
done