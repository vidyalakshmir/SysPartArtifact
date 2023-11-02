BASEDIR=$(pwd)
mkdir -p binaries/httpd/0
mkdir -p binaries/httpd/1
mkdir -p binaries/httpd/2
mkdir -p binaries/httpd/3
mkdir -p output/httpd/0/pin
mkdir -p output/httpd/1/pin
mkdir -p output/httpd/2/pin
mkdir -p output/httpd/3/pin

tar -xvf ../zips/httpd-2.4.39.tar.gz
cd httpd-2.4.39/

export CFLAGS="-O0"
./configure --prefix=$BASEDIR/binaries/httpd/0 --with-apr=$BASEDIR/libapr/0/apr-1.7.0/ --with-apr-util=$BASEDIR/libapr-util/0/apr-util-1.6.1/	
make
make install

cd ..
rm -rf httpd-2.4.39/

tar -xvf ../zips/httpd-2.4.39.tar.gz
cd httpd-2.4.39/

export CFLAGS="-O1"
./configure --prefix=$BASEDIR/binaries/httpd/1 --with-apr=$BASEDIR/libapr/1/apr-1.7.0/ --with-apr-util=$BASEDIR/libapr-util/1/apr-util-1.6.1/	
make
make install

cd ..
rm -rf httpd-2.4.39/

tar -xvf ../zips/httpd-2.4.39.tar.gz
cd httpd-2.4.39/

export CFLAGS="-O2"
./configure --prefix=$BASEDIR/binaries/httpd/2 --with-apr=$BASEDIR/libapr/2/apr-1.7.0/ --with-apr-util=$BASEDIR/libapr-util/2/apr-util-1.6.1/
make
make install

cd ..
rm -rf httpd-2.4.39/

tar -xvf ../zips/httpd-2.4.39.tar.gz
cd httpd-2.4.39/

export CFLAGS="-O3"
./configure --prefix=$BASEDIR/binaries/httpd/3 --with-apr=$BASEDIR/libapr/3/apr-1.7.0/ --with-apr-util=$BASEDIR/libapr-util/3/apr-util-1.6.1/
make
make install

cd ..
rm -rf httpd-2.4.39/


