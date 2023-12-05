BASEDIR=$(pwd)
mkdir -p binaries/lighttpd/0
mkdir -p binaries/lighttpd/1
mkdir -p binaries/lighttpd/2
mkdir -p binaries/lighttpd/3
mkdir -p output/lighttpd/0/pin
mkdir -p output/lighttpd/1/pin
mkdir -p output/lighttpd/2/pin
mkdir -p output/lighttpd/3/pin
tar -xvf ../zips/lighttpd-1.4.54.tar.gz
cd lighttpd-1.4.54/
export CFLAGS="-O0"
CURDIR=$BASEDIR/binaries/lighttpd/0
CC=clang  AR=llvm-ar LD=/usr/bin/ld CFLAGS="-flto -O0 -fpic" LDFLAGS=-pie ./configure --prefix=$CURDIR
make 
make install

cd ..

cd binaries/lighttpd/0
mkdir config log webpages cache run
sudo chown lighttpd log
cp -r $BASEDIR/../../../binaries/build_files/lighttpd/* config/
sed "s#TOFILL#$CURDIR#g" config/lighttpd.conf > temp.txt
mv temp.txt config/lighttpd.conf
sed "s#TOFILL#$CURDIR#g" config/conf.d/Makefile > temp.txt
mv temp.txt config/conf.d/Makefile

cd ../../../

rm -rf lighttpd-1.4.54/

tar -xvf ../zips/lighttpd-1.4.54.tar.gz
cd lighttpd-1.4.54/
export CFLAGS="-O1"

CURDIR=$BASEDIR/binaries/lighttpd/1
CC=clang  AR=llvm-ar LD=/usr/bin/ld CFLAGS="-flto -O1 -fpic" LDFLAGS=-pie ./configure --prefix=$CURDIR
make 
make install

cd ..

cd binaries/lighttpd/1
mkdir config log webpages cache run
sudo chown lighttpd log
cp -r $BASEDIR/../../../binaries/build_files/lighttpd/* config/
sed "s#TOFILL#$CURDIR#g" config/lighttpd.conf > temp.txt
mv temp.txt config/lighttpd.conf
sed "s#TOFILL#$CURDIR#g" config/conf.d/Makefile > temp.txt
mv temp.txt config/conf.d/Makefile

cd ../../../

rm -rf lighttpd-1.4.54/

tar -xvf ../zips/lighttpd-1.4.54.tar.gz
cd lighttpd-1.4.54/
export CFLAGS="-O2"
CURDIR=$BASEDIR/binaries/lighttpd/2
CC=clang  AR=llvm-ar LD=/usr/bin/ld CFLAGS="-flto -O2 -fpic" LDFLAGS=-pie ./configure --prefix=$CURDIR
make 
make install

cd ..

cd binaries/lighttpd/2
mkdir config log webpages cache run
sudo chown lighttpd log
cp -r $BASEDIR/../../../binaries/build_files/lighttpd/* config/
sed "s#TOFILL#$CURDIR#g" config/lighttpd.conf > temp.txt
mv temp.txt config/lighttpd.conf
sed "s#TOFILL#$CURDIR#g" config/conf.d/Makefile > temp.txt
mv temp.txt config/conf.d/Makefile

cd ../../../

rm -rf lighttpd-1.4.54/

tar -xvf ../zips/lighttpd-1.4.54.tar.gz
cd lighttpd-1.4.54/
export CFLAGS="-O3"

CURDIR=$BASEDIR/binaries/lighttpd/3
CC=clang  AR=llvm-ar LD=/usr/bin/ld CFLAGS="-flto -O3 -fpic" LDFLAGS=-pie ./configure --prefix=$CURDIR

make 
make install

cd ..

cd binaries/lighttpd/3
mkdir config log webpages cache run
sudo chown lighttpd log
cp -r $BASEDIR/../../../binaries/build_files/lighttpd/* config/
sed "s#TOFILL#$CURDIR#g" config/lighttpd.conf > temp.txt
mv temp.txt config/lighttpd.conf
sed "s#TOFILL#$CURDIR#g" config/conf.d/Makefile > temp.txt
mv temp.txt config/conf.d/Makefile

cd ../../../
rm -rf lighttpd-1.4.54/
