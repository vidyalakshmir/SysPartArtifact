BASEDIR=$(pwd)
mkdir -p binaries/httpd/0
mkdir -p binaries/httpd/1
mkdir -p binaries/httpd/2
mkdir -p binaries/httpd/3
mkdir -p output/httpd/0/pin
mkdir -p output/httpd/1/pin
mkdir -p output/httpd/2/pin
mkdir -p output/httpd/3/pin
chmod +x libapr/0/apr-1.7.0/libtool
chmod +x libapr/1/apr-1.7.0/libtool
chmod +x libapr/2/apr-1.7.0/libtool
chmod +x libapr/3/apr-1.7.0/libtool
tar -xvf ../zips/httpd-2.4.39.tar.gz
cd httpd-2.4.39/

export CC=clang  
export AR=llvm-ar 
export RANLIB=llvm-ranlib
export NM=llvm-nm
export LD=/usr/bin/ld 
export CFLAGS="-flto -O0 -fpic" 
export LDFLAGS="-flto -Wl -pie"
./configure --prefix=$BASEDIR/binaries/httpd/0 --with-apr=$BASEDIR/libapr/0/apr-1.7.0/ --with-apr-util=$BASEDIR/libapr-util/0/apr-util-1.6.1/	
make -j 8
make install

cd ..
rm -rf httpd-2.4.39/

tar -xvf ../zips/httpd-2.4.39.tar.gz
cd httpd-2.4.39/

export CC=clang  
export AR=llvm-ar 
export LD=/usr/bin/ld 
export CFLAGS="-flto -O1 -fpic" 
export LDFLAGS="-flto -Wl -pie"
./configure --prefix=$BASEDIR/binaries/httpd/1 --with-apr=$BASEDIR/libapr/1/apr-1.7.0/ --with-apr-util=$BASEDIR/libapr-util/1/apr-util-1.6.1/	
make -j 8
make install

cd ..
rm -rf httpd-2.4.39/

tar -xvf ../zips/httpd-2.4.39.tar.gz
cd httpd-2.4.39/

export CC=clang  
export AR=llvm-ar 
export LD=/usr/bin/ld 
export CFLAGS="-flto -O2 -fpic" 
export LDFLAGS="-flto -Wl -pie"
./configure --prefix=$BASEDIR/binaries/httpd/2 --with-apr=$BASEDIR/libapr/2/apr-1.7.0/ --with-apr-util=$BASEDIR/libapr-util/2/apr-util-1.6.1/
make -j 8
make install

cd ..
rm -rf httpd-2.4.39/

tar -xvf ../zips/httpd-2.4.39.tar.gz
cd httpd-2.4.39/

export CC=clang  
export AR=llvm-ar 
export LD=/usr/bin/ld 
export CFLAGS="-flto -O3 -fpic" 
export LDFLAGS="-flto -Wl -pie"
./configure --prefix=$BASEDIR/binaries/httpd/3 --with-apr=$BASEDIR/libapr/3/apr-1.7.0/ --with-apr-util=$BASEDIR/libapr-util/3/apr-util-1.6.1/
make -j 8
make install

cd ..
rm -rf httpd-2.4.39/


