#!/bin/bash
echo ""
echo "RUNNING STATIC LOOP ANALYSIS ON BIND"
echo "--------------------------------------"
echo ""
./compiler_opt_static_loop_analysis.sh gcc bind
./compiler_opt_static_loop_analysis.sh clang bind

echo ""
echo "RUNNING STATIC LOOP ANALYSIS ON HTTPD"
echo "--------------------------------------"
echo ""
./compiler_opt_static_loop_analysis.sh gcc httpd
./compiler_opt_static_loop_analysis.sh clang httpd

echo ""
echo "RUNNING STATIC LOOP ANALYSIS ON LIGHTTPD"
echo "-----------------------------------------"
echo ""

./compiler_opt_static_loop_analysis.sh gcc lighttpd
./compiler_opt_static_loop_analysis.sh clang lighttpd

echo ""
echo "RUNNING STATIC LOOP ANALYSIS ON MEMCACHED"
echo "------------------------------------------"
echo ""

./compiler_opt_static_loop_analysis.sh gcc memcached
./compiler_opt_static_loop_analysis.sh clang memcached

echo ""
echo "RUNNING STATIC LOOP ANALYSIS ON NGINX"
echo "--------------------------------------"
echo ""

./compiler_opt_static_loop_analysis.sh gcc nginx
./compiler_opt_static_loop_analysis.sh clang nginx

echo ""
echo "RUNNING STATIC LOOP ANALYSIS ON REDIS"
echo "--------------------------------------"
echo ""

./compiler_opt_static_loop_analysis.sh gcc redis
./compiler_opt_static_loop_analysis.sh clang redis

echo ""
echo "RUNNING DYNAMIC LOOP ANALYSIS ON BIND"
echo "-------------------------------------"
echo ""

./compiler_opt_dynamic_dominant_loop_detection.sh gcc bind
./compiler_opt_dynamic_dominant_loop_detection.sh clang bind

echo ""
echo "RUNNING DYNAMIC LOOP ANALYSIS ON HTTPD"
echo "---------------------------------------"
echo ""

./compiler_opt_dynamic_dominant_loop_detection.sh gcc httpd
./compiler_opt_dynamic_dominant_loop_detection.sh clang httpd

echo ""
echo "RUNNING DYNAMIC LOOP ANALYSIS ON LIGHTTPD"
echo "------------------------------------------"
echo ""

./compiler_opt_dynamic_dominant_loop_detection.sh gcc lighttpd
./compiler_opt_dynamic_dominant_loop_detection.sh clang lighttpd

echo ""
echo "RUNNING DYNAMIC LOOP ANALYSIS ON MEMCACHED"
echo "-------------------------------------------"
echo ""

./compiler_opt_dynamic_dominant_loop_detection.sh gcc memcached
./compiler_opt_dynamic_dominant_loop_detection.sh clang memcached

echo ""
echo "RUNNING DYNAMIC LOOP ANALYSIS ON NGINX"
echo "----------------------------------------"
echo ""

./compiler_opt_dynamic_dominant_loop_detection.sh gcc nginx
./compiler_opt_dynamic_dominant_loop_detection.sh clang nginx

echo ""
echo "RUNNING DYNAMIC LOOP ANALYSIS ON REDIS"
echo "---------------------------------------"
echo ""

./compiler_opt_dynamic_dominant_loop_detection.sh gcc redis
./compiler_opt_dynamic_dominant_loop_detection.sh clang redis

echo ""
echo "GENERATING SYSCALLS OF BIND"
echo "----------------------------"
echo ""

./compiler_opt_syscall_generation.sh gcc bind
./compiler_opt_syscall_generation.sh clang bind

echo ""
echo "GENERATING SYSCALLS OF HTTPD"
echo "-----------------------------"
echo ""

./compiler_opt_syscall_generation.sh gcc httpd
./compiler_opt_syscall_generation.sh clang httpd

echo ""
echo "GENERATING SYSCALLS OF LIGHTTPD"
echo "--------------------------------"
echo ""

./compiler_opt_syscall_generation.sh gcc lighttpd
./compiler_opt_syscall_generation.sh clang lighttpd

echo ""
echo "GENERATING SYSCALLS OF MEMCACHED"
echo "----------------------------------"
echo ""

./compiler_opt_syscall_generation.sh gcc memcached
./compiler_opt_syscall_generation.sh clang memcached

echo ""
echo "GENERATING SYSCALLS OF NGINX"
echo "-----------------------------"
echo ""

./compiler_opt_syscall_generation.sh gcc nginx
./compiler_opt_syscall_generation.sh clang nginx

echo ""
echo "GENERATING SYSCALLS OF REDIS"
echo "-----------------------------"
echo ""

./compiler_opt_syscall_generation.sh gcc redis
./compiler_opt_syscall_generation.sh clang redis
