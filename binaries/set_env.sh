#!/bin/bash
cur_dir=$(pwd)
binary=$cur_dir/bind9/bind-install/sbin/named
echo "export BIND=$binary" >> ~/.bashrc
binary=$cur_dir/httpd-2.4.39/httpd-install/bin/httpd
echo "export HTTPD=$binary" >> ~/.bashrc
binary=$cur_dir/lighttpd-1.4.54/lighttpd-install/sbin/lighttpd
echo "export LIGHTTPD=$binary" >> ~/.bashrc
binary=$cur_dir/memcached/memcached-install/bin/memcached
echo "export MEMCACHED=$binary" >> ~/.bashrc
binary=$cur_dir/nginx-1.17.1/nginx-install/sbin/nginx
echo "export NGINX=$binary" >> ~/.bashrc
binary=$cur_dir/redis-5.0.7/src/redis-server
echo "export REDIS=$binary" >> ~/.bashrc
source ~/.bashrc