BASEDIR=$(pwd)
mkdir -p libevent/0
mkdir -p libevent/1
mkdir -p libevent/2
mkdir -p libevent/3
mkdir -p binaries/libevent/0/
mkdir -p binaries/libevent/1/
mkdir -p binaries/libevent/2/
mkdir -p binaries/libevent/3

cd libevent/0
tar -xvf ../../../zips/libevent-2.1.11-stable.tar.gz
cd libevent-2.1.11-stable
export CFLAGS="-O0"
./configure --prefix=$BASEDIR/binaries/libevent/0
make
make install
cd ../../

cd 1
tar -xvf ../../../zips/libevent-2.1.11-stable.tar.gz
cd libevent-2.1.11-stable
export CFLAGS="-O1"
./configure --prefix=$BASEDIR/binaries/libevent/1
make
make install
cd ../../


cd 2
tar -xvf ../../../zips/libevent-2.1.11-stable.tar.gz
cd libevent-2.1.11-stable
export CFLAGS="-O2"
./configure --prefix=$BASEDIR/binaries/libevent/2
make
make install
cd ../../

cd 3
tar -xvf ../../../zips/libevent-2.1.11-stable.tar.gz
cd libevent-2.1.11-stable
export CFLAGS="-O3"
./configure --prefix=$BASEDIR/binaries/libevent/3
make
make install
cd ../../