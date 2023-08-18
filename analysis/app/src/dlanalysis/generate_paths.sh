dir1=$(realpath ../../../../binaries)
pathdir=$dir1"/httpd-2.4.39"
echo $pathdir >> pathlist_httpd.txt
pathdir2=$dir1"/lighttpd-1.4.54"
echo $pathdir2 >> pathlist_lighttpd.txt
pathdir3=$dir1"/memcached-sasl"
echo $pathdir3 >> pathlist_memcached.txt
pathdir4=$dir1"/bind9"
echo $pathdir4 >> pathlist_named.txt
pathdir5=$dir1"/nginx-1.17.1"
echo $pathdir5 >> pathlist_nginx.txt
pathdir6=$dir1"/redis-5.0.7"
echo $pathdir6 >> pathlist_redis-server.txt
