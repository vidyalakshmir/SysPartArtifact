BASEDIR=$(pwd)
mkdir -p libapr-util/0
mkdir -p libapr-util/1
mkdir -p libapr-util/2
mkdir -p libapr-util/3

cd libapr-util/0

tar -xvf ../../../zips/apr-util-1.6.1.tar.gz
cd apr-util-1.6.1
mkdir install
export CFLAGS="-O0"
./configure --with-apr=$BASEDIR/libapr/0/apr-1.7.0/ --prefix=$BASEDIR/binaries/libapr-util/0/apr-util-1.6.1/install
make
make install
cd ../..
cd 1

tar -xvf ../../../zips/apr-util-1.6.1.tar.gz
cd apr-util-1.6.1
export CFLAGS="-O1"
./configure --with-apr=$BASEDIR/libapr/1/apr-1.7.0/ --prefix=$BASEDIR/binaries/libapr-util/1/apr-util-1.6.1/install
make
make install
cd ../..
cd 2

tar -xvf ../../../zips/apr-util-1.6.1.tar.gz
cd apr-util-1.6.1
export CFLAGS="-O2"
./configure --with-apr=$BASEDIR/libapr/2/apr-1.7.0/ --prefix=$BASEDIR/binaries/libapr-util/2/apr-util-1.6.1/install
make
make install
cd ../..
cd 3

tar -xvf ../../../zips/apr-util-1.6.1.tar.gz
cd apr-util-1.6.1
export CFLAGS="-O3"
./configure --with-apr=$BASEDIR/libapr/3/apr-1.7.0/ --prefix=$BASEDIR/binaries/libapr-util/3/apr-util-1.6.1/install
make
make install
cd ../../../../
