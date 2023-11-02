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
export CC=clang  
export AR=llvm-ar 
export RANLIB=llvm-ranlib
export LD=/usr/bin/ld 
export CFLAGS="-flto -O0 -fpic" 
export LDFLAGS="-flto -Wl -pie"
./configure --prefix=$BASEDIR/binaries/libevent/0
make -j 8
make install
cd ../../

cd 1
tar -xvf ../../../zips/libevent-2.1.11-stable.tar.gz
cd libevent-2.1.11-stable
export CC=clang  
export AR=llvm-ar 
export LD=/usr/bin/ld 
export CFLAGS="-flto -O1 -fpic" 
export LDFLAGS="-flto -Wl -pie"
./configure --prefix=$BASEDIR/binaries/libevent/1
make -j 8
make install
cd ../../


cd 2
tar -xvf ../../../zips/libevent-2.1.11-stable.tar.gz
cd libevent-2.1.11-stable
export CC=clang  
export AR=llvm-ar 
export LD=/usr/bin/ld 
export CFLAGS="-flto -O2 -fpic" 
export LDFLAGS="-flto -Wl -pie"
./configure --prefix=$BASEDIR/binaries/libevent/2
make -j 8
make install
cd ../../

cd 3
tar -xvf ../../../zips/libevent-2.1.11-stable.tar.gz
cd libevent-2.1.11-stable
export CC=clang  
export AR=llvm-ar 
export LD=/usr/bin/ld 
export CFLAGS="-flto -O3 -fpic" 
export LDFLAGS="-flto -Wl -pie"
./configure --prefix=$BASEDIR/binaries/libevent/3
make -j 8
make install
cd ../../