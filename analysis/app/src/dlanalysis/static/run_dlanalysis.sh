#!/bin/bash
#src/dlanalysis/static/run_dlanalysis.sh src/dlanalysis/binaries 	

#BIN_PATH=$1
OUT_PATH=../../../../../outputs

BINARY_NAME=(redis-server nginx httpd lighttpd memcached named)
BINARY=("$BIND","$HTTPD","$LIGHTTPD","$MEMCACHED","$NGINX","$REDIS")
for i in "${BINARY_NAME[@]}" 
do
	echo "$i"
	DLOUT="$OUT_PATH/$i/dloutput"
	echo "mkdir -p $DLOUT"
	mkdir -p $DLOUT
	
	echo "./syspart -p $BINARY -i -t $OUT_PATH/$i/typearmor/typearmor_parsed.txt -s main -a 6,dlopen@@GLIBC_2.2.5,7"
	./syspart -p $BINARY -i -t $OUT_PATH/$i/typearmor/typearmor_parsed.txt -s main -a 6,dlopen@@GLIBC_2.2.5,7 > $DLOUT/dlopen_static.txt && echo OK
	
	echo "./syspart -p $BINARY -i -t $OUT_PATH/$i/typearmor/typearmor_parsed.txt -s main -a 6,dlsym,6"
	./syspart -p $BINARY -i -t $OUT_PATH/$i/typearmor/typearmor_parsed.txt -s main -a 6,dlsym,6 > $DLOUT/dlsym_static.txt && echo OK
	
	src/dlanalysis/static/generate_libnames.sh $i
	echo "src/dlanalysis/static/match_libs_with_syms.sh $DLOUT/dlsym_static.txt > $DLOUT/libraries_matching_syms.txt"
	src/dlanalysis/static/match_libs_with_syms.sh $DLOUT/dlsym_static.txt $i > $DLOUT/libraries_matching_syms.txt && echo OK
	#./syspart -p $BIN_PATH/$I -i -s main -a 8,$DLOUT/$I/libraries_matching_syms.txt && echo OK
done
