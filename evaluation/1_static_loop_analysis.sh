cd ../analysis/app
echo "Building the tools"
cp ../tools/src_files/elfdynamic.cpp ../tools/egalito/src/elf/elfdynamic.cpp
make
mkdir -p ../outputs/bind
mkdir -p ../outputs/httpd
mkdir -p ../outputs/lighttpd
mkdir -p ../outputs/memcached
mkdir -p ../outputs/nginx
mkdir -p ../outputs/redis

echo "Running static loop analysis on"
echo "Bind Server ...."
./loops $BIND > ../../outputs/bind/temp.out
grep '^FUNC' ../../outputs/bind/temp.out > ../../outputs/bind/functions.out
grep -v '^FUNC' ../../outputs/bind/temp.out > ../../outputs/bind/loops.out
echo "Output written to ../outputs/bind/loops.out"
echo "Httpd Server ...."
./loops $HTTPD > ../../outputs/httpd/temp.out
grep '^FUNC' ../../outputs/httpd/temp.out > ../../outputs/httpd/functions.out
grep -v '^FUNC' ../../outputs/httpd/temp.out > ../../outputs/httpd/loops.out
echo "Output written to ../outputs/httpd/loops.out"

echo "Lighttpd Server ...."
./loops $LIGHTTPD > ../../outputs/lighttpd/temp.out
grep '^FUNC' ../../outputs/lighttpd/temp.out > ../../outputs/lighttpd/functions.out
grep -v '^FUNC' ../../outputs/lighttpd/temp.out > ../../outputs/lighttpd/loops.out
echo "Output written to ../outputs/lighttpd/loops.out"

echo "Memcached Server ...."
./loops $MEMCACHED > ../../outputs/memcached/temp.out
grep '^FUNC' ../../outputs/memcached/temp.out > ../../outputs/memcached/functions.out
grep -v '^FUNC' ../../outputs/memcached/temp.out > ../../outputs/memcached/loops.out
echo "Output written to ../outputs/memcached/loops.out"

echo "Redis Server ...."
./loops $REDIS > ../../outputs/redis/temp.out
grep '^FUNC' ../../outputs/redis/temp.out > ../../outputs/redis/functions.out
grep -v '^FUNC' ../../outputs/redis/temp.out > ../../outputs/redis/loops.out
echo "Output written to ../outputs/redis/loops.out"

echo "Nginx Server ...."
./loops $NGINX > ../../outputs/nginx/temp.out
grep '^FUNC' ../../outputs/nginx/temp.out > ../../outputs/nginx/functions.out
grep -v '^FUNC' ../../outputs/nginx/temp.out > ../../outputs/nginx/loops.out
echo "Output written to ../outputs/nginx/loops.out"

echo "STATIC LOOP ANALYSIS FINISHED"
