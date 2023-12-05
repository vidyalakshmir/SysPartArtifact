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
	echo "System calls of mainloop of lighttpd with gcc and -O0 is :"
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
	echo "System calls of mainloop of lighttpd with gcc and -O1 is :"
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
	echo "System calls of mainloop of lighttpd with gcc and -O2 is :"
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
	echo "System calls of mainloop of lighttpd with gcc and -O3 is :"
	cat $LIGHTTPD_OUT/mainloop_syscalls.out
fi

