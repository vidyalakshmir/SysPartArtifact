BASEDIR=$(pwd)
mkdir -p libapr/0
mkdir -p libapr/1
mkdir -p libapr/2
mkdir -p libapr/3

cd libapr/0
tar -xvf ../../../zips/apr-1.7.0.tar.gz
cd apr-1.7.0
export CC=clang  
export AR=llvm-ar 
export RANLIB=llvm-ranlib
export LD=/usr/bin/ld 
export CFLAGS="-flto -O0 -fpic" 
export LDFLAGS="-flto -Wl -pie"
touch libtoolT
./configure --prefix=$BASEDIR/binaries/libapr/0/apr-1.7.0/install
make
make test
make install
sed 's#NM="/usr/bin/nm -B"#NM="/usr/bin/llvm-nm -B"#' libtool > temp.txt
mv temp.txt libtool
sed 's#\$global_symbol_pipe |##' libtool > temp.txt
mv temp.txt libtool
cd ../..
cd 1

tar -xvf ../../../zips/apr-1.7.0.tar.gz
cd apr-1.7.0
export CC=clang  
export AR=llvm-ar 
export LD=/usr/bin/ld 
export CFLAGS="-flto -O1 -fpic" 
export LDFLAGS="-flto -Wl -pie"
touch libtoolT
./configure --prefix=$BASEDIR/binaries/libapr/1/apr-1.7.0/install
make
make test
make install
sed 's#NM="/usr/bin/nm -B"#NM="/usr/bin/llvm-nm -B"#' libtool > temp.txt
mv temp.txt libtool
sed 's#\$global_symbol_pipe |##' libtool > temp.txt
mv temp.txt libtool
cd ../..
cd 2

tar -xvf ../../../zips/apr-1.7.0.tar.gz
cd apr-1.7.0
export CC=clang  
export AR=llvm-ar 
export LD=/usr/bin/ld 
export CFLAGS="-flto -O2 -fpic" 
export LDFLAGS="-flto -Wl -pie"
touch libtoolT
./configure --prefix=$BASEDIR/binaries/libapr/2/apr-1.7.0/install
make
make test
make install
sed 's#NM="/usr/bin/nm -B"#NM="/usr/bin/llvm-nm -B"#' libtool > temp.txt
mv temp.txt libtool
sed 's#\$global_symbol_pipe |##' libtool > temp.txt
mv temp.txt libtool
cd ../..
cd 3

tar -xvf ../../../zips/apr-1.7.0.tar.gz
cd apr-1.7.0
export CC=clang  
export AR=llvm-ar 
export LD=/usr/bin/ld 
export CFLAGS="-flto -O3 -fpic" 
export LDFLAGS="-flto -Wl -pie"
touch libtoolT
./configure --prefix=$BASEDIR/binaries/libapr/3/apr-1.7.0/install
make
make test
make install
sed 's#NM="/usr/bin/nm -B"#NM="/usr/bin/llvm-nm -B"#' libtool > temp.txt
mv temp.txt libtool
sed 's#\$global_symbol_pipe |##' libtool > temp.txt
mv temp.txt libtool
cd ../../../../

