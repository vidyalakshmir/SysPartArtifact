BASEDIR=$(pwd)
mkdir -p libapr-util/0
mkdir -p libapr-util/1
mkdir -p libapr-util/2
mkdir -p libapr-util/3

cd libapr-util/0

tar -xvf ../../../zips/apr-util-1.6.1.tar.gz
cd apr-util-1.6.1
export CC=clang  
export AR=llvm-ar 
export RANLIB=llvm-ranlib
export NM=llvm-nm
export LD=/usr/bin/ld 
export CFLAGS="-flto -O0 -fpic" 
export LDFLAGS="-flto -Wl -pie"
./configure --with-apr=$BASEDIR/libapr/0/apr-1.7.0/ --prefix=$BASEDIR/binaries/libapr-util/0/apr-util-1.6.1/install
make -j 8
make install
cd ../..
cd 1

tar -xvf ../../../zips/apr-util-1.6.1.tar.gz
cd apr-util-1.6.1
export CC=clang  
export AR=llvm-ar 
export LD=/usr/bin/ld 
export CFLAGS="-flto -O1 -fpic" 
export LDFLAGS="-flto -Wl -pie"
./configure --with-apr=$BASEDIR/libapr/1/apr-1.7.0/ --prefix=$BASEDIR/binaries/libapr-util/1/apr-util-1.6.1/install
make -j 8
make install
cd ../..
cd 2

tar -xvf ../../../zips/apr-util-1.6.1.tar.gz
cd apr-util-1.6.1
export CC=clang  
export AR=llvm-ar 
export LD=/usr/bin/ld 
export CFLAGS="-flto -O2 -fpic" 
export LDFLAGS="-flto -Wl -pie"
./configure --with-apr=$BASEDIR/libapr/2/apr-1.7.0/ --prefix=$BASEDIR/binaries/libapr-util/2/apr-util-1.6.1/install
make -j 8
make install
cd ../..
cd 3

tar -xvf ../../../zips/apr-util-1.6.1.tar.gz
cd apr-util-1.6.1
export CC=clang  
export AR=llvm-ar 
export LD=/usr/bin/ld 
export CFLAGS="-flto -O3 -fpic" 
export LDFLAGS="-flto -Wl -pie"
./configure --with-apr=$BASEDIR/libapr/3/apr-1.7.0/ --prefix=$BASEDIR/binaries/libapr-util/3/apr-util-1.6.1/install
make -j 8
make install
cd ../../../../
