#!/bin/bash

./compiler_opt_static_loop_analysis.sh gcc bind
./compiler_opt_dynamic_dominant_loop_detection.sh gcc bind
./compiler_opt_syscall_generation.sh gcc bind

./compiler_opt_static_loop_analysis.sh clang bind
./compiler_opt_dynamic_dominant_loop_detection.sh clang bind
./compiler_opt_syscall_generation.sh clang bind

./compiler_opt_static_loop_analysis.sh gcc httpd
./compiler_opt_dynamic_dominant_loop_detection.sh gcc httpd
./compiler_opt_syscall_generation.sh gcc httpd

./compiler_opt_static_loop_analysis.sh clang httpd
./compiler_opt_dynamic_dominant_loop_detection.sh clang httpd
./compiler_opt_syscall_generation.sh clang httpd

./compiler_opt_static_loop_analysis.sh gcc lighttpd
./compiler_opt_dynamic_dominant_loop_detection.sh gcc lighttpd
./compiler_opt_syscall_generation.sh gcc lighttpd

./compiler_opt_static_loop_analysis.sh clang lighttpd
./compiler_opt_dynamic_dominant_loop_detection.sh clang lighttpd
./compiler_opt_syscall_generation.sh clang lighttpd


./compiler_opt_static_loop_analysis.sh gcc memcached
./compiler_opt_dynamic_dominant_loop_detection.sh gcc memcached
./compiler_opt_syscall_generation.sh gcc memcached

./compiler_opt_static_loop_analysis.sh clang memcached
./compiler_opt_dynamic_dominant_loop_detection.sh clang memcached
./compiler_opt_syscall_generation.sh clang memcached

./compiler_opt_static_loop_analysis.sh gcc nginx
./compiler_opt_dynamic_dominant_loop_detection.sh gcc nginx
./compiler_opt_syscall_generation.sh gcc nginx

./compiler_opt_static_loop_analysis.sh clang nginx
./compiler_opt_dynamic_dominant_loop_detection.sh clang nginx
./compiler_opt_syscall_generation.sh clang nginx

./compiler_opt_static_loop_analysis.sh gcc redis
./compiler_opt_dynamic_dominant_loop_detection.sh gcc redis
./compiler_opt_syscall_generation.sh gcc redis

./compiler_opt_static_loop_analysis.sh clang redis
./compiler_opt_dynamic_dominant_loop_detection.sh clang redis
./compiler_opt_syscall_generation.sh clang redis

