#!/bin/bash
export OUT=/home/syspart/SysPartArtifact/outputs

mkdir -p $OUT/bind/syscall_results
sed 's/\[//' $OUT/bind/syscalls.json | sed 's/\]//' | sed 's/,/\n/g' > $OUT/bind/syscall_results/mainloop_sysno.txt
python3 src/scripts/convert_sysno_to_name.py $OUT/bind/syscall_results/mainloop_sysno.txt | sort > $OUT/bind/syscall_results/mainloop_sysnames.txt

mkdir -p $OUT/httpd/syscall_results
sed 's/\[//' $OUT/httpd/syscalls.json | sed 's/\]//' | sed 's/,/\n/g' > $OUT/httpd/syscall_results/mainloop_sysno.txt
python3 src/scripts/convert_sysno_to_name.py $OUT/httpd/syscall_results/mainloop_sysno.txt | sort > $OUT/httpd/syscall_results/mainloop_sysnames.txt

mkdir -p $OUT/lighttpd/syscall_results
sed 's/\[//' $OUT/lighttpd/syscalls.json | sed 's/\]//' | sed 's/,/\n/g' > $OUT/lighttpd/syscall_results/mainloop_sysno.txt
python3 src/scripts/convert_sysno_to_name.py $OUT/lighttpd/syscall_results/mainloop_sysno.txt | sort > $OUT/lighttpd/syscall_results/mainloop_sysnames.txt

mkdir -p $OUT/memcached/syscall_results
sed 's/\[//' $OUT/memcached/syscalls.json | sed 's/\]//' | sed 's/,/\n/g' > $OUT/memcached/syscall_results/mainloop_sysno.txt
python3 src/scripts/convert_sysno_to_name.py $OUT/memcached/syscall_results/mainloop_sysno.txt | sort > $OUT/memcached/syscall_results/mainloop_sysnames.txt

mkdir -p $OUT/nginx/syscall_results
sed 's/\[//' $OUT/nginx/syscalls.json | sed 's/\]//' | sed 's/,/\n/g' > $OUT/nginx/syscall_results/mainloop_sysno.txt
python3 src/scripts/convert_sysno_to_name.py $OUT/nginx/syscall_results/mainloop_sysno.txt | sort > $OUT/nginx/syscall_results/mainloop_sysnames.txt

mkdir -p $OUT/redis/syscall_results
sed 's/\[//' $OUT/redis/syscalls.json | sed 's/\]//' | sed 's/,/\n/g' > $OUT/redis/syscall_results/mainloop_sysno.txt
python3 src/scripts/convert_sysno_to_name.py $OUT/redis/syscall_results/mainloop_sysno.txt | sort > $OUT/redis/syscall_results/mainloop_sysnames.txt


