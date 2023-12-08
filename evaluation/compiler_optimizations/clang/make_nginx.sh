BASEDIR=$(pwd)
mkdir -p binaries/nginx/0
mkdir -p binaries/nginx/1
mkdir -p binaries/nginx/2
mkdir -p binaries/nginx/3
mkdir -p output/nginx/0/pin
mkdir -p output/nginx/1/pin
mkdir -p output/nginx/2/pin
mkdir -p output/nginx/3/pin
tar -xvf ../zips/nginx-1.17.1.tar.gz
cd nginx-1.17.1/
CURDIR=$BASEDIR/binaries/nginx/0
mkdir -p $CURDIR
CC=clang  AR=llvm-ar LD=/usr/bin/ld CFLAGS="-flto -O0 -fpic" LDFLAGS=-pie ./configure --with-http_ssl_module --with-ld-opt="-flto -Wl -pie" --prefix=$CURDIR --sbin-path=$CURDIR/sbin/nginx --conf-path=$CURDIR/conf/nginx.conf --error-log-path=$CURDIR/logs/error.log --http-log-path=$CURDIR/logs/access.log --pid-path=$CURDIR/nginx.pid

make && make install

cd ../

rm -rf nginx-1.17.1/

tar -xvf ../zips/nginx-1.17.1.tar.gz

cd nginx-1.17.1/



CURDIR=$BASEDIR/binaries/nginx/1
mkdir -p $CURDIR
CC=clang  AR=llvm-ar LD=/usr/bin/ld CFLAGS="-flto -O1 -fpic" LDFLAGS=-pie ./configure --with-http_ssl_module --with-ld-opt="-flto -Wl -pie" --prefix=$CURDIR --sbin-path=$CURDIR/sbin/nginx --conf-path=$CURDIR/conf/nginx.conf --error-log-path=$CURDIR/logs/error.log --http-log-path=$CURDIR/logs/access.log --pid-path=$CURDIR/nginx.pid


make && make install

cd ../

rm -rf nginx-1.17.1/

tar -xvf ../zips/nginx-1.17.1.tar.gz

cd nginx-1.17.1/

CURDIR=$BASEDIR/binaries/nginx/2
mkdir -p $CURDIR
CC=clang  AR=llvm-ar LD=/usr/bin/ld CFLAGS="-flto -O2 -fpic" LDFLAGS=-pie ./configure --with-http_ssl_module --with-ld-opt="-flto -Wl -pie" --prefix=$CURDIR --sbin-path=$CURDIR/sbin/nginx --conf-path=$CURDIR/conf/nginx.conf --error-log-path=$CURDIR/logs/error.log --http-log-path=$CURDIR/logs/access.log --pid-path=$CURDIR/nginx.pid


make && make install

cd ../

rm -rf nginx-1.17.1/

tar -xvf ../zips/nginx-1.17.1.tar.gz

cd nginx-1.17.1/

CURDIR=$BASEDIR/binaries/nginx/3
mkdir -p $CURDIR
CC=clang  AR=llvm-ar LD=/usr/bin/ld CFLAGS="-flto -O3 -fpic" LDFLAGS=-pie ./configure --with-http_ssl_module --with-ld-opt="-flto -Wl -pie" --prefix=$CURDIR --sbin-path=$CURDIR/sbin/nginx --conf-path=$CURDIR/conf/nginx.conf --error-log-path=$CURDIR/logs/error.log --http-log-path=$CURDIR/logs/access.log --pid-path=$CURDIR/nginx.pid


make && make install


