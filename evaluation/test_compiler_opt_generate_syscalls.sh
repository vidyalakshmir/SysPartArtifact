#!/bin/bash   
compiler=$1
app=$2  

BASEDIR=$(pwd)
EVALDIR=$(dirname $BASEDIR)

cd ../analysis/app
cp ../tools/src_files/elfdynamic_libc_2.24.cpp ../tools/egalito/src/elf/elfdynamic.cpp
cp ../tools/src_files/nss_libc_2.24.cpp src/nss.cpp
make
#base_dir=$(dirname $(dirname $(pwd)))
#OUT="$base_dir/outputs"


    
if [[ "$app" == "lighttpd" ]]           
then
    	LIGHTTPD=$BASEDIR/compiler_optimizations/$compiler/binaries/lighttpd/0/sbin/lighttpd
	LIGHTTPD_OUT=$EVALDIR/outputs/lighttpd/opt/$compiler/0
	if [[ "$compiler" == "gcc" ]]
	then
		./syspart -p $LIGHTTPD -i -s main -a 2,f7ed,main > $LIGHTTPD_OUT/syscalls.out
	else
		./syspart -p $LIGHTTPD -i -s main -a 2,a925,main > $LIGHTTPD_OUT/syscalls.out
	fi
	grep 'JSON' $LIGHTTPD_OUT/syscalls.out | awk {'print $2'} > $LIGHTTPD_OUT/syscalls.json
	grep 'PARTITION_SIZE' $LIGHTTPD_OUT/syscalls.out | awk {'print $2'} > $LIGHTTPD_OUT/partition_size.out
	grep -w 'MAIN' $LIGHTTPD_OUT/syscalls.out | awk {'print $2'} > $LIGHTTPD_OUT/main_syscalls.out
	grep -w 'MAINLOOP' $LIGHTTPD_OUT/syscalls.out | awk {'print $2'} > $LIGHTTPD_OUT/mainloop_syscalls.out
	echo "System calls of mainloop of lighttpd with ${compiler} and -O0 is :"
	cat $LIGHTTPD_OUT/mainloop_syscalls.out

	LIGHTTPD=$BASEDIR/compiler_optimizations/$compiler/binaries/lighttpd/1/sbin/lighttpd
	LIGHTTPD_OUT=$EVALDIR/outputs/lighttpd/opt/$compiler/1
	if [[ "$compiler" == "gcc" ]]
	then
		./syspart -p $LIGHTTPD -i -s main -a 2,d1a9,main > $LIGHTTPD_OUT/syscalls.out
	else
		./syspart -p $LIGHTTPD -i -s main -a 2,be76,server_main_loop > $LIGHTTPD_OUT/syscalls.out
	fi
	grep 'JSON' $LIGHTTPD_OUT/syscalls.out | awk {'print $2'} > $LIGHTTPD_OUT/syscalls.json
	grep 'PARTITION_SIZE' $LIGHTTPD_OUT/syscalls.out | awk {'print $2'} > $LIGHTTPD_OUT/partition_size.out
	grep -w 'MAIN' $LIGHTTPD_OUT/syscalls.out | awk {'print $2'} > $LIGHTTPD_OUT/main_syscalls.out
	grep -w 'MAINLOOP' $LIGHTTPD_OUT/syscalls.out | awk {'print $2'} > $LIGHTTPD_OUT/mainloop_syscalls.out
	echo "System calls of mainloop of lighttpd with ${compiler} and -O1 is :"
	cat $LIGHTTPD_OUT/mainloop_syscalls.out

	LIGHTTPD=$BASEDIR/compiler_optimizations/$compiler/binaries/lighttpd/2/sbin/lighttpd
	LIGHTTPD_OUT=$EVALDIR/outputs/lighttpd/opt/$compiler/2
	if [[ "$compiler" == "gcc" ]]
	then
		./syspart -p $LIGHTTPD -i -s main -a 2,c32a,main > $LIGHTTPD_OUT/syscalls.out
	else
		./syspart -p $LIGHTTPD -i -s main -a 2,c456,server_main_loop > $LIGHTTPD_OUT/syscalls.out
	fi
	grep 'JSON' $LIGHTTPD_OUT/syscalls.out | awk {'print $2'} > $LIGHTTPD_OUT/syscalls.json
	grep 'PARTITION_SIZE' $LIGHTTPD_OUT/syscalls.out | awk {'print $2'} > $LIGHTTPD_OUT/partition_size.out
	grep -w 'MAIN' $LIGHTTPD_OUT/syscalls.out | awk {'print $2'} > $LIGHTTPD_OUT/main_syscalls.out
	grep -w 'MAINLOOP' $LIGHTTPD_OUT/syscalls.out | awk {'print $2'} > $LIGHTTPD_OUT/mainloop_syscalls.out
	echo "System calls of mainloop of lighttpd with ${compiler} and -O2 is :"
	cat $LIGHTTPD_OUT/mainloop_syscalls.out

	LIGHTTPD=$BASEDIR/compiler_optimizations/$compiler/binaries/lighttpd/3/sbin/lighttpd
	LIGHTTPD_OUT=$EVALDIR/outputs/lighttpd/opt/$compiler/3
	if [[ "$compiler" == "gcc" ]]
	then
		./syspart -p $LIGHTTPD -i -s main -a 2,c32a,main > $LIGHTTPD_OUT/syscalls.out
	else
		./syspart -p $LIGHTTPD -i -s main -a 2,c9e6,server_main_loop > $LIGHTTPD_OUT/syscalls.out
	fi
	grep 'JSON' $LIGHTTPD_OUT/syscalls.out | awk {'print $2'} > $LIGHTTPD_OUT/syscalls.json
	grep 'PARTITION_SIZE' $LIGHTTPD_OUT/syscalls.out | awk {'print $2'} > $LIGHTTPD_OUT/partition_size.out
	grep -w 'MAIN' $LIGHTTPD_OUT/syscalls.out | awk {'print $2'} > $LIGHTTPD_OUT/main_syscalls.out
	grep -w 'MAINLOOP' $LIGHTTPD_OUT/syscalls.out | awk {'print $2'} > $LIGHTTPD_OUT/mainloop_syscalls.out
	echo "System calls of mainloop of lighttpd with ${compiler} and -O3 is :"
	cat $LIGHTTPD_OUT/mainloop_syscalls.out
elif [[ "$app" == "redis" ]]
then
        REDIS=$BASEDIR/compiler_optimizations/$compiler/binaries/redis/0/redis-server
        REDIS_OUT=$EVALDIR/outputs/redis/opt/$compiler/0
        if [[ "$compiler" == "gcc" ]]
        then
                ./syspart -p $REDIS -s main -a 2,3615d,aeMain > $REDIS_OUT/syscalls.out
        else
                ./syspart -p $REDIS -s main -a 2,352f7,aeMain > $REDIS_OUT/syscalls.out
        fi
        grep 'JSON' $REDIS_OUT/syscalls.out | awk {'print $2'} > $REDIS_OUT/syscalls.json
        grep 'PARTITION_SIZE' $REDIS_OUT/syscalls.out | awk {'print $2'} > $REDIS_OUT/partition_size.out
        grep -w 'MAIN' $REDIS_OUT/syscalls.out | awk {'print $2'} > $REDIS_OUT/main_syscalls.out
        grep -w 'MAINLOOP' $REDIS_OUT/syscalls.out | awk {'print $2'} > $REDIS_OUT/mainloop_syscalls.out
        echo "System calls of mainloop of redis with ${compiler} and -O0 is :"
        cat $REDIS_OUT/mainloop_syscalls.out

        REDIS=$BASEDIR/compiler_optimizations/$compiler/binaries/redis/1/redis-server
        REDIS_OUT=$EVALDIR/outputs/redis/opt/$compiler/1
        if [[ "$compiler" == "gcc" ]]
        then
                ./syspart -p $REDIS -s main -a 2,33d03,aeMain > $REDIS_OUT/syscalls.out
        else
                ./syspart -p $REDIS -s main -a 2,43509,main > $REDIS_OUT/syscalls.out
        fi
        grep 'JSON' $REDIS_OUT/syscalls.out | awk {'print $2'} > $REDIS_OUT/syscalls.json
        grep 'PARTITION_SIZE' $REDIS_OUT/syscalls.out | awk {'print $2'} > $REDIS_OUT/partition_size.out
        grep -w 'MAIN' $REDIS_OUT/syscalls.out | awk {'print $2'} > $REDIS_OUT/main_syscalls.out
        grep -w 'MAINLOOP' $REDIS_OUT/syscalls.out | awk {'print $2'} > $REDIS_OUT/mainloop_syscalls.out
        echo "System calls of mainloop of redis with ${compiler} and -O1 is :"
        cat $REDIS_OUT/mainloop_syscalls.out

         REDIS=$BASEDIR/compiler_optimizations/$compiler/binaries/redis/2/redis-server
        REDIS_OUT=$EVALDIR/outputs/redis/opt/$compiler/2
        if [[ "$compiler" == "gcc" ]]
        then
                ./syspart -p $REDIS -s main -a 2,34aa0,aeMain > $REDIS_OUT/syscalls.out
        else
                ./syspart -p $REDIS -s main -a 2,4500e,main > $REDIS_OUT/syscalls.out
        fi
        grep 'JSON' $REDIS_OUT/syscalls.out | awk {'print $2'} > $REDIS_OUT/syscalls.json
        grep 'PARTITION_SIZE' $REDIS_OUT/syscalls.out | awk {'print $2'} > $REDIS_OUT/partition_size.out
        grep -w 'MAIN' $REDIS_OUT/syscalls.out | awk {'print $2'} > $REDIS_OUT/main_syscalls.out
        grep -w 'MAINLOOP' $REDIS_OUT/syscalls.out | awk {'print $2'} > $REDIS_OUT/mainloop_syscalls.out
        echo "System calls of mainloop of redis with ${compiler} and -O2 is :"
        cat $REDIS_OUT/mainloop_syscalls.out

        REDIS=$BASEDIR/compiler_optimizations/$compiler/binaries/redis/3/redis-server
        REDIS_OUT=$EVALDIR/outputs/redis/opt/$compiler/3
        if [[ "$compiler" == "gcc" ]]
        then
                ./syspart -p $REDIS -s main -a 2,39110,aeMain > $REDIS_OUT/syscalls.out
        else
                ./syspart -p $REDIS -s main -a 2,45e3e,main > $REDIS_OUT/syscalls.out
        fi
        grep 'JSON' $REDIS_OUT/syscalls.out | awk {'print $2'} > $REDIS_OUT/syscalls.json
        grep 'PARTITION_SIZE' $REDIS_OUT/syscalls.out | awk {'print $2'} > $REDIS_OUT/partition_size.out
        grep -w 'MAIN' $REDIS_OUT/syscalls.out | awk {'print $2'} > $REDIS_OUT/main_syscalls.out
        grep -w 'MAINLOOP' $REDIS_OUT/syscalls.out | awk {'print $2'} > $REDIS_OUT/mainloop_syscalls.out
        echo "System calls of mainloop of redis with ${compiler} and -O3 is :"
        cat $REDIS_OUT/mainloop_syscalls.out
elif [[ "$app" == "memcached" ]]
then
        MEMCACHED=$BASEDIR/compiler_optimizations/$compiler/binaries/memcached/0/bin/memcached
        MEMCACHED_OUT=$EVALDIR/outputs/memcached/opt/$compiler/0
        if [[ "$compiler" == "gcc" ]]
        then
                ./syspart -p $MEMCACHED -i -s main -a 2,25b32,event_base_loop > $MEMCACHED_OUT/syscalls.out
        else
                ./syspart -p $MEMCACHED -i -s main -a 2,275a6,event_base_loop > $MEMCACHED_OUT/syscalls.out
        fi
        grep 'JSON' $MEMCACHED_OUT/syscalls.out | awk {'print $2'} > $MEMCACHED_OUT/syscalls.json
        grep 'PARTITION_SIZE' $MEMCACHED_OUT/syscalls.out | awk {'print $2'} > $MEMCACHED_OUT/partition_size.out
        grep -w 'MAIN' $MEMCACHED_OUT/syscalls.out | awk {'print $2'} > $MEMCACHED_OUT/main_syscalls.out
        grep -w 'MAINLOOP' $MEMCACHED_OUT/syscalls.out | awk {'print $2'} > $MEMCACHED_OUT/mainloop_syscalls.out
        echo "System calls of mainloop of memcached with ${compiler} and -O0 is :"
        cat $MEMCACHED_OUT/mainloop_syscalls.out

        MEMCACHED=$BASEDIR/compiler_optimizations/$compiler/binaries/memcached/1/bin/memcached
        MEMCACHED_OUT=$EVALDIR/outputs/memcached/opt/$compiler/1
        if [[ "$compiler" == "gcc" ]]
        then
                ./syspart -p $MEMCACHED -i -s main -a 2,216d8,event_base_loop > $MEMCACHED_OUT/syscalls.out
        else
                ./syspart -p $MEMCACHED -i -s main -a 2,1d1b0,event_base_loop > $MEMCACHED_OUT/syscalls.out
        fi
        grep 'JSON' $MEMCACHED_OUT/syscalls.out | awk {'print $2'} > $MEMCACHED_OUT/syscalls.json
        grep 'PARTITION_SIZE' $MEMCACHED_OUT/syscalls.out | awk {'print $2'} > $MEMCACHED_OUT/partition_size.out
        grep -w 'MAIN' $MEMCACHED_OUT/syscalls.out | awk {'print $2'} > $MEMCACHED_OUT/main_syscalls.out
        grep -w 'MAINLOOP' $MEMCACHED_OUT/syscalls.out | awk {'print $2'} > $MEMCACHED_OUT/mainloop_syscalls.out
        echo "System calls of mainloop of memcached with ${compiler} and -O1 is :"
        cat $MEMCACHED_OUT/mainloop_syscalls.out

         MEMCACHED=$BASEDIR/compiler_optimizations/$compiler/binaries/memcached/2/bin/memcached
        MEMCACHED_OUT=$EVALDIR/outputs/memcached/opt/$compiler/2
        if [[ "$compiler" == "gcc" ]]
        then
                ./syspart -p $MEMCACHED -i -s main -a 2,2232b,event_base_loop > $MEMCACHED_OUT/syscalls.out
        else
                ./syspart -p $MEMCACHED -i -s main -a 2,22b00,event_base_loop > $MEMCACHED_OUT/syscalls.out
        fi
        grep 'JSON' $MEMCACHED_OUT/syscalls.out | awk {'print $2'} > $MEMCACHED_OUT/syscalls.json
        grep 'PARTITION_SIZE' $MEMCACHED_OUT/syscalls.out | awk {'print $2'} > $MEMCACHED_OUT/partition_size.out
        grep -w 'MAIN' $MEMCACHED_OUT/syscalls.out | awk {'print $2'} > $MEMCACHED_OUT/main_syscalls.out
        grep -w 'MAINLOOP' $MEMCACHED_OUT/syscalls.out | awk {'print $2'} > $MEMCACHED_OUT/mainloop_syscalls.out
        echo "System calls of mainloop of memcached with ${compiler} and -O2 is :"
        cat $MEMCACHED_OUT/mainloop_syscalls.out

        MEMCACHED=$BASEDIR/compiler_optimizations/$compiler/binaries/memcached/3/bin/memcached
        MEMCACHED_OUT=$EVALDIR/outputs/memcached/opt/$compiler/3
        if [[ "$compiler" == "gcc" ]]
        then
                ./syspart -p $MEMCACHED -i -s main -a 2,24c0b,event_base_loop > $MEMCACHED_OUT/syscalls.out
        else
                ./syspart -p $MEMCACHED -i -s main -a 2,259f0,event_base_loop > $MEMCACHED_OUT/syscalls.out
        fi
        grep 'JSON' $MEMCACHED_OUT/syscalls.out | awk {'print $2'} > $MEMCACHED_OUT/syscalls.json
        grep 'PARTITION_SIZE' $MEMCACHED_OUT/syscalls.out | awk {'print $2'} > $MEMCACHED_OUT/partition_size.out
        grep -w 'MAIN' $MEMCACHED_OUT/syscalls.out | awk {'print $2'} > $MEMCACHED_OUT/main_syscalls.out
        grep -w 'MAINLOOP' $MEMCACHED_OUT/syscalls.out | awk {'print $2'} > $MEMCACHED_OUT/mainloop_syscalls.out
        echo "System calls of mainloop of memcached with ${compiler} and -O3 is :"
        cat $MEMCACHED_OUT/mainloop_syscalls.out
elif [[ "$app" == "nginx" ]]
then
        NGINX=$BASEDIR/compiler_optimizations/$compiler/binaries/nginx/0/sbin/nginx
        NGINX_OUT=$EVALDIR/outputs/nginx/opt/$compiler/0
        if [[ "$compiler" == "gcc" ]]
        then
                ./syspart -p $NGINX -s main -a 2,62b1e,ngx_worker_process_cycle > $NGINX_OUT/syscalls.out
        else
                ./syspart -p $NGINX -s main -a 2,5aba7,ngx_worker_process_cycle > $NGINX_OUT/syscalls.out
        fi
        grep 'JSON' $NGINX_OUT/syscalls.out | awk {'print $2'} > $NGINX_OUT/syscalls.json
        grep 'PARTITION_SIZE' $NGINX_OUT/syscalls.out | awk {'print $2'} > $NGINX_OUT/partition_size.out
        grep -w 'MAIN' $NGINX_OUT/syscalls.out | awk {'print $2'} > $NGINX_OUT/main_syscalls.out
        grep -w 'MAINLOOP' $NGINX_OUT/syscalls.out | awk {'print $2'} > $NGINX_OUT/mainloop_syscalls.out
        echo "System calls of mainloop of nginx with ${compiler} and -O0 is :"
        cat $NGINX_OUT/mainloop_syscalls.out

        NGINX=$BASEDIR/compiler_optimizations/$compiler/binaries/nginx/1/sbin/nginx
        NGINX_OUT=$EVALDIR/outputs/nginx/opt/$compiler/1
        if [[ "$compiler" == "gcc" ]]
        then
                ./syspart -p $NGINX -s main -a 2,45ed6,ngx_worker_process_cycle > $NGINX_OUT/syscalls.out
        else
                ./syspart -p $NGINX -s main -a 2,509dd,ngx_worker_process_cycle > $NGINX_OUT/syscalls.out
        fi
        grep 'JSON' $NGINX_OUT/syscalls.out | awk {'print $2'} > $NGINX_OUT/syscalls.json
        grep 'PARTITION_SIZE' $NGINX_OUT/syscalls.out | awk {'print $2'} > $NGINX_OUT/partition_size.out
        grep -w 'MAIN' $NGINX_OUT/syscalls.out | awk {'print $2'} > $NGINX_OUT/main_syscalls.out
        grep -w 'MAINLOOP' $NGINX_OUT/syscalls.out | awk {'print $2'} > $NGINX_OUT/mainloop_syscalls.out
        echo "System calls of mainloop of nginx with ${compiler} and -O1 is :"
        cat $NGINX_OUT/mainloop_syscalls.out

         NGINX=$BASEDIR/compiler_optimizations/$compiler/binaries/nginx/2/sbin/nginx
        NGINX_OUT=$EVALDIR/outputs/nginx/opt/$compiler/2
        if [[ "$compiler" == "gcc" ]]
        then
                ./syspart -p $NGINX -s main -a 2,46498,ngx_worker_process_cycle > $NGINX_OUT/syscalls.out
        else
                ./syspart -p $NGINX -s main -a 2,4f2ed,ngx_worker_process_cycle > $NGINX_OUT/syscalls.out
        fi
        grep 'JSON' $NGINX_OUT/syscalls.out | awk {'print $2'} > $NGINX_OUT/syscalls.json
        grep 'PARTITION_SIZE' $NGINX_OUT/syscalls.out | awk {'print $2'} > $NGINX_OUT/partition_size.out
        grep -w 'MAIN' $NGINX_OUT/syscalls.out | awk {'print $2'} > $NGINX_OUT/main_syscalls.out
        grep -w 'MAINLOOP' $NGINX_OUT/syscalls.out | awk {'print $2'} > $NGINX_OUT/mainloop_syscalls.out
        echo "System calls of mainloop of nginx with ${compiler} and -O2 is :"
        cat $NGINX_OUT/mainloop_syscalls.out

        NGINX=$BASEDIR/compiler_optimizations/$compiler/binaries/nginx/3/sbin/nginx
        NGINX_OUT=$EVALDIR/outputs/nginx/opt/$compiler/3
        if [[ "$compiler" == "gcc" ]]
        then
                ./syspart -p $NGINX -s main -a 2,4dde8,ngx_worker_process_cycle > $NGINX_OUT/syscalls.out
        else
                ./syspart -p $NGINX -s main -a 2,5134d,ngx_worker_process_cycle > $NGINX_OUT/syscalls.out
        fi
        grep 'JSON' $NGINX_OUT/syscalls.out | awk {'print $2'} > $NGINX_OUT/syscalls.json
        grep 'PARTITION_SIZE' $NGINX_OUT/syscalls.out | awk {'print $2'} > $NGINX_OUT/partition_size.out
        grep -w 'MAIN' $NGINX_OUT/syscalls.out | awk {'print $2'} > $NGINX_OUT/main_syscalls.out
        grep -w 'MAINLOOP' $NGINX_OUT/syscalls.out | awk {'print $2'} > $NGINX_OUT/mainloop_syscalls.out
        echo "System calls of mainloop of nginx with ${compiler} and -O3 is :"
        cat $NGINX_OUT/mainloop_syscalls.out
elif [[ "$app" == "httpd" ]]
then
        HTTPD=$BASEDIR/compiler_optimizations/$compiler/binaries/httpd/0/bin/httpd
        HTTPD_OUT=$EVALDIR/outputs/httpd/opt/$compiler/0
        if [[ "$compiler" == "gcc" ]]
        then
                ./syspart -p $HTTPD -s main -a 2,9d96d,child_main > $HTTPD_OUT/syscalls.out
        else
                ./syspart -p $HTTPD -s main -a 2,a30ff,child_main > $HTTPD_OUT/syscalls.out
        fi

        grep 'JSON' $HTTPD_OUT/syscalls.out | awk {'print $2'} > $HTTPD_OUT/syscalls.json
        grep 'PARTITION_SIZE' $HTTPD_OUT/syscalls.out | awk {'print $2'} > $HTTPD_OUT/partition_size.out
        grep -w 'MAIN' $HTTPD_OUT/syscalls.out | awk {'print $2'} > $HTTPD_OUT/main_syscalls.out
        grep -w 'MAINLOOP' $HTTPD_OUT/syscalls.out | awk {'print $2'} > $HTTPD_OUT/mainloop_syscalls.out
        echo "System calls of mainloop of httpd with ${compiler} and -O0 is :"
        cat $HTTPD_OUT/mainloop_syscalls.out

        HTTPD=$BASEDIR/compiler_optimizations/$compiler/binaries/httpd/1/bin/httpd
        HTTPD_OUT=$EVALDIR/outputs/httpd/opt/$compiler/1
        if [[ "$compiler" == "gcc" ]]
        then
                ./syspart -p $HTTPD -s main -a 2,7690e,child_main > $HTTPD_OUT/syscalls.out
        else
                ./syspart -p $HTTPD -s main -a 2,75260,child_main > $HTTPD_OUT/syscalls.out
        fi
        grep 'JSON' $HTTPD_OUT/syscalls.out | awk {'print $2'} > $HTTPD_OUT/syscalls.json
        grep 'PARTITION_SIZE' $HTTPD_OUT/syscalls.out | awk {'print $2'} > $HTTPD_OUT/partition_size.out
        grep -w 'MAIN' $HTTPD_OUT/syscalls.out | awk {'print $2'} > $HTTPD_OUT/main_syscalls.out
        grep -w 'MAINLOOP' $HTTPD_OUT/syscalls.out | awk {'print $2'} > $HTTPD_OUT/mainloop_syscalls.out
        echo "System calls of mainloop of httpd with ${compiler} and -O1 is :"
        cat $HTTPD_OUT/mainloop_syscalls.out

         HTTPD=$BASEDIR/compiler_optimizations/$compiler/binaries/httpd/2/bin/httpd
        HTTPD_OUT=$EVALDIR/outputs/httpd/opt/$compiler/2
        if [[ "$compiler" == "gcc" ]]
        then
                ./syspart -p $HTTPD -s main -a 2,7a3c8,child_main > $HTTPD_OUT/syscalls.out
        else
                ./syspart -p $HTTPD -s main -a 2,85fe0,child_main > $HTTPD_OUT/syscalls.out
        fi
        grep 'JSON' $HTTPD_OUT/syscalls.out | awk {'print $2'} > $HTTPD_OUT/syscalls.json
        grep 'PARTITION_SIZE' $HTTPD_OUT/syscalls.out | awk {'print $2'} > $HTTPD_OUT/partition_size.out
        grep -w 'MAIN' $HTTPD_OUT/syscalls.out | awk {'print $2'} > $HTTPD_OUT/main_syscalls.out
        grep -w 'MAINLOOP' $HTTPD_OUT/syscalls.out | awk {'print $2'} > $HTTPD_OUT/mainloop_syscalls.out
        echo "System calls of mainloop of httpd with ${compiler} and -O2 is :"
        cat $HTTPD_OUT/mainloop_syscalls.out

        HTTPD=$BASEDIR/compiler_optimizations/$compiler/binaries/httpd/3/bin/httpd
        HTTPD_OUT=$EVALDIR/outputs/httpd/opt/$compiler/3
        if [[ "$compiler" == "gcc" ]]
        then
                ./syspart -p $HTTPD -s main -a 2,8fe16,child_main > $HTTPD_OUT/syscalls.out
        else
                ./syspart -p $HTTPD -s main -a 2,8a350,child_main > $HTTPD_OUT/syscalls.out
        fi
        grep 'JSON' $HTTPD_OUT/syscalls.out | awk {'print $2'} > $HTTPD_OUT/syscalls.json
        grep 'PARTITION_SIZE' $HTTPD_OUT/syscalls.out | awk {'print $2'} > $HTTPD_OUT/partition_size.out
        grep -w 'MAIN' $HTTPD_OUT/syscalls.out | awk {'print $2'} > $HTTPD_OUT/main_syscalls.out
        grep -w 'MAINLOOP' $HTTPD_OUT/syscalls.out | awk {'print $2'} > $HTTPD_OUT/mainloop_syscalls.out
        echo "System calls of mainloop of httpd with ${compiler} and -O3 is :"
        cat $HTTPD_OUT/mainloop_syscalls.out
fi


