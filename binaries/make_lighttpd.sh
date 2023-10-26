#!/bin/sh

tar -xvf lighttpd-1.4.54.tar.gz
cd lighttpd-1.4.54/

CURDIR=$(pwd)/lighttpd-install

mkdir $CURDIR

./configure --prefix=$CURDIR

make

make install

cd $CURDIR
mkdir config log webpages cache run
sudo chown lighttpd log
cp -r ../../build_files/lighttpd/* config/
sed "s#TOFILL#$CURDIR#g" config/lighttpd.conf > temp.txt
mv temp.txt config/lighttpd.conf
sed "s#TOFILL#$CURDIR#g" config/conf.d/Makefile > temp.txt
mv temp.txt config/conf.d/Makefile

