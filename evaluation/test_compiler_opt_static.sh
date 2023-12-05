#!/bin/bash   
compiler=$1
app=$2  
BASEDIR=$(pwd)
EVALDIR=$(dirname $BASEDIR)
cd ../analysis/app
echo "Building the tools"
cp ../tools/src_files/elfdynamic.cpp ../tools/egalito/src/elf/elfdynamic.cpp
make


for i in 0 1 2 3; do     
	if [[ "$app" == "bind" ]]           
	then
		export BINARYAPP=$BASEDIR/compiler_optimizations/$compiler/binaries/bind/$i/sbin/named
		export OUT=$EVALDIR/outputs/bind/opt/$compiler/$i
	elif [[ "$app" == "httpd" ]]           
	then
	             export BINARYAPP=$BASEDIR/compiler_optimizations/$compiler/binaries/httpd/$i/bin/httpd
		export OUT=$EVALDIR/outputs/httpd/opt/$compiler/$i
	elif [[ "$app" == "lighttpd" ]]           
	then
                export BINARYAPP=$BASEDIR/compiler_optimizations/$compiler/binaries/lighttpd/$i/sbin/lighttpd
		export OUT=$EVALDIR/outputs/lighttpd/opt/$compiler/$i
	elif [[ "$app" == "memcached" ]]           
	then
                export BINARYAPP=$BASEDIR/compiler_optimizations/$compiler/binaries/memcached/$i/bin/memcached
		export OUT=$EVALDIR/outputs/memcached/opt/$compiler/$i
	elif [[ "$app" == "nginx" ]]           
	then
                export BINARYAPP=$BASEDIR/compiler_optimizations/$compiler/binaries/nginx/$i/sbin/nginx
		export OUT=$EVALDIR/outputs/nginx/opt/$compiler/$i
	elif [[ "$app" == "redis" ]]           
	then
                export BINARYAPP=$BASEDIR/compiler_optimizations/$compiler/binaries/redis/$i/redis-server
		export OUT=$EVALDIR/outputs/redis/opt/$compiler/$i
	fi
       mkdir -p $OUT              
	echo "Running static loop analysis on $BINARYAPP"
	./loops $BINARYAPP > $OUT/temp.out
    grep '^FUNC' $OUT/temp.out > $OUT/functions.out
    grep -v '^FUNC' $OUT/temp.out > $OUT/loops.out
	echo "Output written to $OUT/loops.out"
done

