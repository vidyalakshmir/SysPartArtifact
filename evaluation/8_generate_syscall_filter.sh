cd ../analysis/app
cp ../tools/src_files/elfdynamic_libc_2.24.cpp ../tools/egalito/src/elf/elfdynamic.cpp
cp ../tools/src_files/nss_libc_2.24.cpp src/nss.cpp
make
base_dir=$(dirname $(dirname $(pwd)))
OUT="$base_dir/outputs"
PIN_ROOT="$base_dir/analysis/tools/pin-3.11-97998-g7ecce2dac-gcc-linux"


echo "GENERATING SYSCALL FILTER FOR BIND .. "
src/scripts/parse_typearmor.sh $OUT/bind/typearmor/
./syspart -p $BIND -i -s main -t ../../outputs/bind/typearmor/typearmor_parsed.txt -a 2,7bb41,main > $OUT/bind/syscalls.out
./syspart -p $BIND -i -s main -t ../../outputs/bind/typearmor/typearmor_parsed.txt -a 7,isc_app_ctxrun > $OUT/bind/serving_syscalls.out
./syspart -p $BIND -i -s main -t ../../outputs/bind/typearmor/typearmor_parsed.txt -a 14 > $OUT/bind/at.out
grep 'JSON' $OUT/bind/syscalls.out | awk {'print $2'} > $OUT/bind/syscalls.json
grep 'PARTITION_SIZE' $OUT/bind/syscalls.out | awk {'print $2'} > $OUT/bind/partition_size.out
grep -w 'MAIN' $OUT/bind/syscalls.out | awk {'print $2'} > $OUT/bind/main_syscalls.out
grep -w 'MAINLOOP' $OUT/bind/syscalls.out | awk {'print $2'} > $OUT/bind/mainloop_syscalls.out
echo "BIND RESULTS : "
echo -n "Partition size : "
cat $OUT/bind/partition_size.out
echo -n "#Syscalls of main() : "
cat $OUT/bind/main_syscalls.out

echo -n "#Syscalls of serving phase : "
grep SIZE $OUT/bind/serving_syscalls.out | awk {'print $2'}

echo -n "#Syscalls of main loop : "
cat $OUT/bind/mainloop_syscalls.out
echo -n "#Syscalls directly accessible from AT : "
ct=$(awk {'print $1'} $OUT/bind/at.out | sort | uniq | wc -l)
((ct=ct-1))
echo $ct

echo "GENERATING SYSCALL FILTER FOR HTTPD .. "
src/scripts/parse_typearmor.sh $OUT/httpd/typearmor/
./syspart -p $HTTPD -i -s main -t ../../outputs/httpd/typearmor/typearmor_parsed.txt -a 2,7a378,child_main > $OUT/httpd/syscalls.out
./syspart -p $HTTPD -i -s main -t ../../outputs/httpd/typearmor/typearmor_parsed.txt -a 7,child_main > $OUT/httpd/serving_syscalls.out
./syspart -p $HTTPD -i -s main -t ../../outputs/httpd/typearmor/typearmor_parsed.txt -a 14 > $OUT/httpd/at.out
grep 'JSON' $OUT/httpd/syscalls.out | awk {'print $2'} > $OUT/httpd/syscalls.json
grep 'PARTITION_SIZE' $OUT/httpd/syscalls.out | awk {'print $2'} > $OUT/httpd/partition_size.out
grep -w 'MAIN' $OUT/httpd/syscalls.out | awk {'print $2'} > $OUT/httpd/main_syscalls.out
grep -w 'MAINLOOP' $OUT/httpd/syscalls.out | awk {'print $2'} > $OUT/httpd/mainloop_syscalls.out
echo "HTTPD RESULTS : "
echo -n "Partition size : "
cat $OUT/httpd/partition_size.out
echo -n "#Syscalls of main() : "
cat $OUT/httpd/main_syscalls.out

echo -n "#Syscalls of serving phase : "
grep SIZE $OUT/httpd/serving_syscalls.out | awk {'print $2'}

echo -n "#Syscalls of main loop : "
cat $OUT/httpd/mainloop_syscalls.out
echo -n "#Syscalls directly accessible from AT : "
ct=$(awk {'print $1'} $OUT/httpd/at.out | sort | uniq | wc -l)
((ct=ct-1))
echo $ct

echo "GENERATING SYSCALL FILTER FOR LIGHTTPD .. "
src/scripts/parse_typearmor.sh $OUT/lighttpd/typearmor/
./syspart -p $LIGHTTPD -i -s main -t ../../outputs/lighttpd/typearmor/typearmor_parsed.txt -a 2,c32a,main > $OUT/lighttpd/syscalls.out
./syspart -p $LIGHTTPD -i -s main -t ../../outputs/lighttpd/typearmor/typearmor_parsed.txt -a 7,server_main_loop > $OUT/lighttpd/serving_syscalls.out
./syspart -p $LIGHTTPD -i -s main -t ../../outputs/httpd/typearmor/typearmor_parsed.txt -a 14 > $OUT/lighttpd/at.out
grep 'JSON' $OUT/lighttpd/syscalls.out | awk {'print $2'} > $OUT/lighttpd/syscalls.json
grep 'PARTITION_SIZE' $OUT/lighttpd/syscalls.out | awk {'print $2'} > $OUT/lighttpd/partition_size.out
grep -w 'MAIN' $OUT/lighttpd/syscalls.out | awk {'print $2'} > $OUT/lighttpd/main_syscalls.out
grep -w 'MAINLOOP' $OUT/lighttpd/syscalls.out | awk {'print $2'} > $OUT/lighttpd/mainloop_syscalls.out
echo "LIGHTTPD RESULTS : "
echo -n "Partition size : "
cat $OUT/lighttpd/partition_size.out
echo -n "#Syscalls of main() : "
cat $OUT/lighttpd/main_syscalls.out

echo -n "#Syscalls of serving phase : "
grep SIZE $OUT/lighttpd/serving_syscalls.out | awk {'print $2'}

echo -n "#Syscalls of main loop : "
cat $OUT/lighttpd/mainloop_syscalls.out
echo -n "#Syscalls directly accessible from AT : "
ct=$(awk {'print $1'} $OUT/lighttpd/at.out | sort | uniq | wc -l)
((ct=ct-1))
echo $ct


echo "GENERATING SYSCALL FILTER FOR MEMCACHED .. "
src/scripts/parse_typearmor.sh $OUT/memcached/typearmor/
./syspart -p $MEMCACHED -i -s main -t ../../outputs/memcached/typearmor/typearmor_parsed.txt -a 2,2232b,event_base_loop > $OUT/memcached/syscalls.out
./syspart -p $MEMCACHED -i -s main -t ../../outputs/memcached/typearmor/typearmor_parsed.txt -a 7,worker_libevent > $OUT/memcached/serving_syscalls.out
./syspart -p $MEMCACHED -i -s main -t ../../outputs/memcached/typearmor/typearmor_parsed.txt -a 14 > $OUT/memcached/at.out
grep 'JSON' $OUT/memcached/syscalls.out | awk {'print $2'} > $OUT/memcached/syscalls.json
grep 'PARTITION_SIZE' $OUT/memcached/syscalls.out | awk {'print $2'} > $OUT/memcached/partition_size.out
grep -w 'MAIN' $OUT/memcached/syscalls.out | awk {'print $2'} > $OUT/memcached/main_syscalls.out
grep -w 'MAINLOOP' $OUT/memcached/syscalls.out | awk {'print $2'} > $OUT/memcached/mainloop_syscalls.out
echo "MEMCACHED RESULTS : "
echo -n "Partition size : "
cat $OUT/memcached/partition_size.out
echo -n "#Syscalls of main() : "
cat $OUT/memcached/main_syscalls.out

echo -n "#Syscalls of serving phase : "
grep SIZE $OUT/memcached/serving_syscalls.out | awk {'print $2'}

echo -n "#Syscalls of main loop : "
cat $OUT/memcached/mainloop_syscalls.out
echo -n "#Syscalls directly accessible from AT : "
ct=$(awk {'print $1'} $OUT/memcached/at.out | sort | uniq | wc -l)
((ct=ct-1))
echo $ct

echo "GENERATING SYSCALL FILTER FOR NGINX .. "
src/scripts/parse_typearmor.sh $OUT/nginx/typearmor/
./syspart -p $NGINX -i -s main -t ../../outputs/nginx/typearmor/typearmor_parsed.txt -a 2,45f06,ngx_worker_process_cycle > $OUT/nginx/syscalls.out
./syspart -p $NGINX -i -s main -t ../../outputs/nginx/typearmor/typearmor_parsed.txt -a 7,ngx_worker_process_cycle > $OUT/nginx/serving_syscalls.out
./syspart -p $NGINX -i -s main -t ../../outputs/memcached/typearmor/typearmor_parsed.txt -a 14 > $OUT/nginx/at.out
grep 'JSON' $OUT/nginx/syscalls.out | awk {'print $2'} > $OUT/nginx/syscalls.json
grep 'PARTITION_SIZE' $OUT/nginx/syscalls.out | awk {'print $2'} > $OUT/nginx/partition_size.out
grep -w 'MAIN' $OUT/nginx/syscalls.out | awk {'print $2'} > $OUT/nginx/main_syscalls.out
grep -w 'MAINLOOP' $OUT/nginx/syscalls.out | awk {'print $2'} > $OUT/nginx/mainloop_syscalls.out
echo "NGINX RESULTS : "
echo -n "Partition size : "
cat $OUT/nginx/partition_size.out
echo -n "#Syscalls of main() : "
cat $OUT/nginx/main_syscalls.out

echo -n "#Syscalls of serving phase : "
grep SIZE $OUT/nginx/serving_syscalls.out | awk {'print $2'}

echo -n "#Syscalls of main loop : "
cat $OUT/nginx/mainloop_syscalls.out
echo -n "#Syscalls directly accessible from AT : "
ct=$(awk {'print $1'} $OUT/nginx/at.out | sort | uniq | wc -l)
((ct=ct-1))
echo $ct

