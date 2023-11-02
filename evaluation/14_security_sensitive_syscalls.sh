#!/bin/bash
mkdir -p criticalsyscalls/sf
mkdir -p criticalsyscalls/tsp
mkdir -p criticalsyscalls/sp

cp ../related_work/sf/bind/allowed_syscalls.out criticalsyscalls/sf/bind_syscalls.out
cp ../related_work/sf/httpd/allowed_syscalls.out criticalsyscalls/sf/httpd_syscalls.out
cp ../related_work/sf/lighttpd/allowed_syscalls.out criticalsyscalls/sf/lighttpd_syscalls.out
cp ../related_work/sf/memcached/allowed_syscalls.out criticalsyscalls/sf/memcached_syscalls.out
cp ../related_work/sf/nginx/allowed_syscalls.out criticalsyscalls/sf/nginx_syscalls.out
cp ../related_work/sf/redis/allowed_syscalls.out criticalsyscalls/sf/redis_syscalls.out

cp ../related_work/tsp/bind/fixed_mainloop.txt criticalsyscalls/tsp/bind_syscalls.out
cp ../related_work/tsp/httpd/fixed_mainloop.txt criticalsyscalls/tsp/httpd_syscalls.out
cp ../related_work/tsp/lighttpd/fixed_mainloop.txt criticalsyscalls/tsp/lighttpd_syscalls.out
cp ../related_work/tsp/memcached/fixed_mainloop.txt criticalsyscalls/tsp/memcached_syscalls.out
cp ../related_work/tsp/nginx/fixed_mainloop.txt criticalsyscalls/tsp/nginx_syscalls.out
cp ../related_work/tsp/redis/fixed_mainloop.txt criticalsyscalls/tsp/redis_syscalls.out

cp ../outputs/nginx/syscall_results/mainloop_sysnames.txt criticalsyscalls/sp/nginx_syscalls.out
cp ../outputs/httpd/syscall_results/mainloop_sysnames.txt criticalsyscalls/sp/httpd_syscalls.out
grep 'SYSCALLS' ../outputs/lighttpd/serving_syscalls.out | awk {'print $2'} | sed 's/\[//' | sed 's/\]//' | sed 's/,/\n/g' | sort | uniq > criticalsyscalls/sp/lighttpd_syscalls.out
cp ../outputs/memcached/syscall_results/mainloop_sysnames.txt criticalsyscalls/sp/memcached_syscalls.out
cp ../outputs/redis/syscall_results/mainloop_sysnames.txt criticalsyscalls/sp/redis_syscalls.out
cp ../outputs/bind/syscall_results/mainloop_sysnames.txt criticalsyscalls/sp/bind_syscalls.out

cd criticalsyscalls/
rm -f criticalsyscalls.out 1.out
grep -w clone  sf/*_syscalls.out | sed 's/_.*:/ /' | sed 's/.*\///' >> 1.out
grep -w execveat  sf/*_syscalls.out | sed 's/_.*:/ /' | sed 's/.*\///' >> 1.out
grep -w execve  sf/*_syscalls.out | sed 's/_.*:/ /' | sed 's/.*\///' >> 1.out
grep -w fork  sf/*_syscalls.out | sed 's/_.*:/ /' | sed 's/.*\///' >> 1.out
grep -w ptrace  sf/*_syscalls.out | sed 's/_.*:/ /' | sed 's/.*\///' >> 1.out
grep -w chmod  sf/*_syscalls.out | sed 's/_.*:/ /' | sed 's/.*\///' >> 1.out
grep -w mprotect  sf/*_syscalls.out | sed 's/_.*:/ /' | sed 's/.*\///' >> 1.out
grep -w setgid  sf/*_syscalls.out | sed 's/_.*:/ /' | sed 's/.*\///' >> 1.out
grep -w setreuid  sf/*_syscalls.out | sed 's/_.*:/ /' | sed 's/.*\///' >> 1.out
grep -w setuid  sf/*_syscalls.out | sed 's/_.*:/ /' | sed 's/.*\///' >> 1.out
grep -w accept4  sf/*_syscalls.out | sed 's/_.*:/ /' | sed 's/.*\///' >> 1.out
grep -w accept  sf/*_syscalls.out | sed 's/_.*:/ /' | sed 's/.*\///' >> 1.out
grep -w bind  sf/*_syscalls.out | sed 's/_.*:/ /' | sed 's/.*\///' >> 1.out
grep -w connect  sf/*_syscalls.out | sed 's/_.*:/ /' | sed 's/.*\///' >> 1.out
grep -w listen  sf/*_syscalls.out | sed 's/_.*:/ /' | sed 's/.*\///' >> 1.out
grep -w recvfrom  sf/*_syscalls.out | sed 's/_.*:/ /' | sed 's/.*\///' >> 1.out
grep -w socket  sf/*_syscalls.out | sed 's/_.*:/ /' | sed 's/.*\///' >> 1.out
awk '{print "SF " $0}' 1.out >> criticalsyscalls.out
rm -f 1.out

grep -w clone  tsp/*_syscalls.out | sed 's/_.*:/ /' | sed 's/.*\///' >> 1.out
grep -w execveat  tsp/*_syscalls.out | sed 's/_.*:/ /' | sed 's/.*\///' >> 1.out
grep -w execve  tsp/*_syscalls.out | sed 's/_.*:/ /' | sed 's/.*\///' >> 1.out
grep -w fork  tsp/*_syscalls.out | sed 's/_.*:/ /' | sed 's/.*\///' >> 1.out
grep -w ptrace  tsp/*_syscalls.out | sed 's/_.*:/ /' | sed 's/.*\///' >> 1.out
grep -w chmod  tsp/*_syscalls.out | sed 's/_.*:/ /' | sed 's/.*\///' >> 1.out
grep -w mprotect  tsp/*_syscalls.out | sed 's/_.*:/ /' | sed 's/.*\///' >> 1.out
grep -w setgid  tsp/*_syscalls.out | sed 's/_.*:/ /' | sed 's/.*\///' >> 1.out
grep -w setreuid  tsp/*_syscalls.out | sed 's/_.*:/ /' | sed 's/.*\///' >> 1.out
grep -w setuid  tsp/*_syscalls.out | sed 's/_.*:/ /' | sed 's/.*\///' >> 1.out
grep -w accept4  tsp/*_syscalls.out | sed 's/_.*:/ /' | sed 's/.*\///' >> 1.out
grep -w accept  tsp/*_syscalls.out | sed 's/_.*:/ /' | sed 's/.*\///' >> 1.out
grep -w bind  tsp/*_syscalls.out | sed 's/_.*:/ /' | sed 's/.*\///' >> 1.out
grep -w connect  tsp/*_syscalls.out | sed 's/_.*:/ /' | sed 's/.*\///' >> 1.out
grep -w listen  tsp/*_syscalls.out | sed 's/_.*:/ /' | sed 's/.*\///' >> 1.out
grep -w recvfrom  tsp/*_syscalls.out | sed 's/_.*:/ /' | sed 's/.*\///' >> 1.out
grep -w socket  tsp/*_syscalls.out | sed 's/_.*:/ /' | sed 's/.*\///' >> 1.out
awk '{print "TSP " $0}' 1.out >> criticalsyscalls.out
rm -f 1.out

grep -w clone  sp/*_syscalls.out | sed 's/_.*:/ /' | sed 's/.*\///' >> 1.out
grep -w execveat  sp/*_syscalls.out | sed 's/_.*:/ /' | sed 's/.*\///' >> 1.out
grep -w execve  sp/*_syscalls.out | sed 's/_.*:/ /' | sed 's/.*\///' >> 1.out
grep -w fork  sp/*_syscalls.out | sed 's/_.*:/ /' | sed 's/.*\///' >> 1.out
grep -w ptrace  sp/*_syscalls.out | sed 's/_.*:/ /' | sed 's/.*\///' >> 1.out
grep -w chmod  sp/*_syscalls.out | sed 's/_.*:/ /' | sed 's/.*\///' >> 1.out
grep -w mprotect  sp/*_syscalls.out | sed 's/_.*:/ /' | sed 's/.*\///' >> 1.out
grep -w setgid  sp/*_syscalls.out | sed 's/_.*:/ /' | sed 's/.*\///' >> 1.out
grep -w setreuid  sp/*_syscalls.out | sed 's/_.*:/ /' | sed 's/.*\///' >> 1.out
grep -w setuid  sp/*_syscalls.out | sed 's/_.*:/ /' | sed 's/.*\///' >> 1.out
grep -w accept4  sp/*_syscalls.out | sed 's/_.*:/ /' | sed 's/.*\///' >> 1.out
grep -w accept  sp/*_syscalls.out | sed 's/_.*:/ /' | sed 's/.*\///' >> 1.out
grep -w bind  sp/*_syscalls.out | sed 's/_.*:/ /' | sed 's/.*\///' >> 1.out
grep -w connect  sp/*_syscalls.out | sed 's/_.*:/ /' | sed 's/.*\///' >> 1.out
grep -w listen  sp/*_syscalls.out | sed 's/_.*:/ /' | sed 's/.*\///' >> 1.out
grep -w recvfrom  sp/*_syscalls.out | sed 's/_.*:/ /' | sed 's/.*\///' >> 1.out
grep -w socket  sp/*_syscalls.out | sed 's/_.*:/ /' | sed 's/.*\///' >> 1.out
awk '{print "SP " $0}' 1.out >> criticalsyscalls.out
rm -f 1.out

python3 printoutput.py criticalsyscalls.out
