BASEDIR=$(pwd)
mkdir -p libapr/0
mkdir -p libapr/1
mkdir -p libapr/2
mkdir -p libapr/3

cd libapr/0
tar -xvf ../../../zips/apr-1.7.0.tar.gz
cd apr-1.7.0
mkdir install
export CFLAGS="-O0"
touch libtoolT
./configure --prefix=$BASEDIR/binaries/libapr/0/apr-1.7.0/install
make
make install
cd ../..
cd 1

tar -xvf ../../../zips/apr-1.7.0.tar.gz
cd apr-1.7.0
mkdir install
export CFLAGS="-O1"
touch libtoolT
./configure --prefix=$BASEDIR/binaries/libapr/1/apr-1.7.0/install
make
make install
cd ../..
cd 2

tar -xvf ../../../zips/apr-1.7.0.tar.gz
cd apr-1.7.0
mkdir install
export CFLAGS="-O2"
touch libtoolT
./configure --prefix=$BASEDIR/binaries/libapr/2/apr-1.7.0/install
make
make install
cd ../..
cd 3

tar -xvf ../../../zips/apr-1.7.0.tar.gz
cd apr-1.7.0
export CFLAGS="-O3"
touch libtoolT
./configure --prefix=$BASEDIR/binaries/libapr/3/apr-1.7.0/install
make
make install
cd ../../../../

