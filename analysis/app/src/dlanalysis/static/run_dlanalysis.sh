#!/bin/bash
#src/dlanalysis/static/run_dlanalysis.sh src/dlanalysis/binaries        

#BIN_PATH=$1
script_dir=$(dirname "$(realpath "$0")")
cp $script_dir/../../../../tools/src_files/nss_libc_2.27.cpp $script_dir/../../nss.cpp
cd $script_dir/../../../
make
cd src/dlanalysis/static
OUT_PATH=$script_dir/../../../../../outputs

BINARY_NAME=(bind httpd lighttpd memcached nginx redis)
BINARY=($BIND $HTTPD $LIGHTTPD $MEMCACHED $NGINX $REDIS)
for i in {0..5}
do
        echo "Dlanalysis for ${BINARY_NAME[i]} server"
        DLOUT="$OUT_PATH/${BINARY_NAME[i]}/dloutput"
        #echo "mkdir -p $DLOUT"
        mkdir -p $DLOUT
        #echo "CURDIR" 
        pwd
        #echo "./syspart -p ${BINARY[i]} -i -t $OUT_PATH/${BINARY_NAME[i]}/typearmor/typearmor_parsed.txt -s main -a 6,dlopen@@GLIBC_2.2.5,7"
        $script_dir/../../../syspart -p ${BINARY[i]} -i -t $OUT_PATH/${BINARY_NAME[i]}/typearmor/typearmor_parsed.txt -s main -a 6,dlopen@@GLIBC_2.2.5,7 > $DLOUT/dlopen_static.txt && echo OK

        #echo "./syspart -p ${BINARY[i]} -i -t $OUT_PATH/${BINARY_NAME[i]}/typearmor/typearmor_parsed.txt -s main -a 6,dlsym,6"
        $script_dir/../../../syspart -p ${BINARY[i]} -i -t $OUT_PATH/$i/typearmor/typearmor_parsed.txt -s main -a 6,dlsym,6 > $DLOUT/dlsym_static.txt && echo OK

        $script_dir/generate_libnames.sh ${BINARY_NAME[i]}
        #echo "src/dlanalysis/static/match_libs_with_syms.sh $DLOUT/dlsym_static.txt > $DLOUT/libraries_matching_syms.txt"
        $script_dir/match_libs_with_syms.sh $DLOUT/dlsym_static.txt ${BINARY_NAME[i]} > $DLOUT/libraries_matching_syms.txt && echo OK
        $script_dir/../../../syspart -p ${BINARY[i]} -i -s main -a 8,$DLOUT/$I/libraries_matching_syms.txt && echo OK
done
