script_dir=$(dirname "$(realpath "$0")")
dir1=$(realpath $script_dir/../../../../binaries)
rm -f pathlist_httpd.txt
rm -f pathlist_lighttpd.txt
rm -f pathlist_memcached.txt
rm -f pathlist_bind.txt
rm -f pathlist_nginx.txt
rm -f pathlist_redis.txt
pathdir=$dir1"/httpd-2.4.39"
echo "/lib/x86_64-linux-gnu" > pathlist_httpd.txt
echo $pathdir >> pathlist_httpd.txt
pathdir2=$dir1"/lighttpd-1.4.54"
echo "/lib/x86_64-linux-gnu" > pathlist_lighttpd.txt
echo $pathdir2 >> pathlist_lighttpd.txt
pathdir3=$dir1"/memcached"
echo "/lib/x86_64-linux-gnu" > pathlist_memcached.txt
echo $pathdir3 >> pathlist_memcached.txt
pathdir4=$dir1"/bind9"
echo "/lib/x86_64-linux-gnu" > pathlist_bind.txt
echo $pathdir4 >> pathlist_bind.txt
pathdir5=$dir1"/nginx-1.17.1"
echo "/lib/x86_64-linux-gnu" > pathlist_nginx.txt
echo $pathdir5 >> pathlist_nginx.txt
pathdir6=$dir1"/redis-5.0.7"
echo "/lib/x86_64-linux-gnu" > pathlist_redis.txt
echo $pathdir6 >> pathlist_redis.txt
