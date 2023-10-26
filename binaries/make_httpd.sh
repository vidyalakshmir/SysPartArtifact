tar -xvf apr-1.7.0.tar.gz
cd apr-1.7.0
APR_DIR=$(pwd)
CURDIR1=$APR_DIR/apr-install
mkdir $CURDIR1
touch libtoolT
./configure --prefix=$CURDIR1
make
make test
make install

cd ../
tar -xvf apr-util-1.6.1.tar.gz
cd apr-util-1.6.1
APR_UTIL_DIR=$(pwd)
CURDIR2=$APR_UTIL_DIR/apr-util-install
mkdir $CURDIR2
./configure --with-apr=$APR_DIR --prefix=$CURDIR2
make
make install

cd ../
tar -xvf httpd-2.4.39.tar.gz
cd httpd-2.4.39/
CURDIR3=$(pwd)/httpd-install
mkdir $CURDIR3
./configure --prefix=$CURDIR3 --with-apr=$APR_DIR --with-apr-util=$APR_UTIL_DIR
make
make install


