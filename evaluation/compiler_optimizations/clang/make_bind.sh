#!/bin/sh
BASEDIR=$(pwd)
mkdir -p binaries/bind/0
mkdir -p binaries/bind/1
mkdir -p binaries/bind/2
mkdir -p binaries/bind/3
mkdir -p output/bind/0/pin
mkdir -p output/bind/1/pin
mkdir -p output/bind/2/pin
mkdir -p output/bind/3/pin

tar -xvf ../zips/bind9.tar.gz
cd bind9/
export PKG_CONFIG_PATH=$BASEDIR/libuv/0/libuv-1.34.0/
export CC=clang  
export AR=llvm-ar 
export RANLIB=llvm-ranlib
export LD=/usr/bin/ld 
export CFLAGS="-flto -O0 -fpic" 
export LDFLAGS="-flto -pie -Wl,-rpath,$BASEDIR/binaries/libuv/0/libuv-1.34.0/install/lib"
$CURDIR2=$BASEDIR/binaries/bind/0
./configure  --without-libxml2 --prefix=$BASEDIR/binaries/bind/0 
make -j 8
make install
cd ../
rm -rf bind9
cp $(BASEDIR)../../binaries/build_files/bind/etc/* $CURDIR2/etc/
sed "s#TOFILL#$CURDIR2#g" $CURDIR2/etc/named.conf.default-zones > temp.txt
mv temp.txt $CURDIR2/etc/named.conf.default-zones

sed "s#TOFILL#$CURDIR2#g" $CURDIR2/etc/named.conf > temp.txt
mv temp.txt $CURDIR2/etc/named.conf

sed "s#TOFILL#$CURDIR2#g" $CURDIR2/etc/named.conf.options > temp.txt
mv temp.txt $CURDIR2/etc/named.conf.options

mkdir $CURDIR2/var/cache
mkdir $CURDIR2/var/run
sudo chown bind:bind $CURDIR2/var/cache
sudo chown bind:bind $CURDIR2/var/run
sudo chown bind:bind $CURDIR2/lib

tar -xvf ../zips/bind9.tar.gz
cd bind9/
export PKG_CONFIG_PATH=$BASEDIR/libuv/1/libuv-1.34.0/
export CC=clang  
export AR=llvm-ar 
export LD=/usr/bin/ld 
export CFLAGS="-flto -O1 -fpic" 
export LDFLAGS="-flto -pie -Wl,-rpath,$BASEDIR/binaries/libuv/0/libuv-1.34.0/install/lib"
$CURDIR2=$BASEDIR/binaries/bind/1
./configure  --without-libxml2 --prefix=$BASEDIR/binaries/bind/1 
make -j 8
make install
cd ../
rm -rf bind9
cp $(BASEDIR)../../binaries/build_files/bind/etc/* $CURDIR2/etc/
sed "s#TOFILL#$CURDIR2#g" $CURDIR2/etc/named.conf.default-zones > temp.txt
mv temp.txt $CURDIR2/etc/named.conf.default-zones

sed "s#TOFILL#$CURDIR2#g" $CURDIR2/etc/named.conf > temp.txt
mv temp.txt $CURDIR2/etc/named.conf

sed "s#TOFILL#$CURDIR2#g" $CURDIR2/etc/named.conf.options > temp.txt
mv temp.txt $CURDIR2/etc/named.conf.options

mkdir $CURDIR2/var/cache
mkdir $CURDIR2/var/run
sudo chown bind:bind $CURDIR2/var/cache
sudo chown bind:bind $CURDIR2/var/run
sudo chown bind:bind $CURDIR2/lib

tar -xvf ../zips/bind9.tar.gz
cd bind9/	
export PKG_CONFIG_PATH=$BASEDIR/libuv/2/libuv-1.34.0
export CC=clang  
export AR=llvm-ar 
export LD=/usr/bin/ld 
export CFLAGS="-flto -O2 -fpic" 
export LDFLAGS="-flto -pie -Wl,-rpath,$BASEDIR/binaries/libuv/0/libuv-1.34.0/install/lib"
$CURDIR2=$BASEDIR/binaries/bind/2
./configure --without-libxml2 --prefix=$BASEDIR/binaries/bind/2 
make -j 8
make install
cd ../
rm -rf bind9
cp $(BASEDIR)../../binaries/build_files/bind/etc/* $CURDIR2/etc/
sed "s#TOFILL#$CURDIR2#g" $CURDIR2/etc/named.conf.default-zones > temp.txt
mv temp.txt $CURDIR2/etc/named.conf.default-zones

sed "s#TOFILL#$CURDIR2#g" $CURDIR2/etc/named.conf > temp.txt
mv temp.txt $CURDIR2/etc/named.conf

sed "s#TOFILL#$CURDIR2#g" $CURDIR2/etc/named.conf.options > temp.txt
mv temp.txt $CURDIR2/etc/named.conf.options

mkdir $CURDIR2/var/cache
mkdir $CURDIR2/var/run
sudo chown bind:bind $CURDIR2/var/cache
sudo chown bind:bind $CURDIR2/var/run
sudo chown bind:bind $CURDIR2/lib

tar -xvf ../zips/bind9.tar.gz
cd bind9/
export PKG_CONFIG_PATH=$BASEDIR/libuv/3/libuv-1.34.0
export CC=clang  
export AR=llvm-ar 
export LD=/usr/bin/ld 
export CFLAGS="-flto -O3 -fpic" 
export LDFLAGS="-flto -pie -Wl,-rpath,$BASEDIR/binaries/libuv/0/libuv-1.34.0/install/lib"
$CURDIR2=$BASEDIR/binaries/bind/3
./configure  --without-libxml2 --prefix=$BASEDIR/binaries/bind/3
make -j 8
make install
cd ../
rm -rf bind9
cp $(BASEDIR)../../binaries/build_files/bind/etc/* $CURDIR2/etc/
sed "s#TOFILL#$CURDIR2#g" $CURDIR2/etc/named.conf.default-zones > temp.txt
mv temp.txt $CURDIR2/etc/named.conf.default-zones

sed "s#TOFILL#$CURDIR2#g" $CURDIR2/etc/named.conf > temp.txt
mv temp.txt $CURDIR2/etc/named.conf

sed "s#TOFILL#$CURDIR2#g" $CURDIR2/etc/named.conf.options > temp.txt
mv temp.txt $CURDIR2/etc/named.conf.options

mkdir $CURDIR2/var/cache
mkdir $CURDIR2/var/run
sudo chown bind:bind $CURDIR2/var/cache
sudo chown bind:bind $CURDIR2/var/run
sudo chown bind:bind $CURDIR2/lib