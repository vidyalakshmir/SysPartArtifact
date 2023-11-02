mkdir -p binaries/redis/0
mkdir -p binaries/redis/1
mkdir -p binaries/redis/2
mkdir -p binaries/redis/3
mkdir -p output/redis/0/pin
mkdir -p output/redis/1/pin
mkdir -p output/redis/2/pin
mkdir -p output/redis/3/pin

tar -xvf ../zips/redis-5.0.7.tar.gz
cd redis-5.0.7
export CFLAGS="-O0"
sed 's#OPTIMIZATION?=-O2#OPTIMIZATION?=-O0#' src/Makefile > temp.txt
mv temp.txt src/Makefile
sed 's#OPTIMIZATION?=-O3#OPTIMIZATION?=-O0#' deps/hiredis/Makefile > temp.txt
mv temp.txt deps/hiredis/Makefile
sed 's#OPT= -Os#OPT= -O0#' deps/linenoise/Makefile > temp.txt
mv temp.txt deps/linenoise/Makefile
sed 's#LUA_CFLAGS+= -O2#LUA_CFLAGS+= -O0#' deps/Makefile > temp.txt
mv temp.txt deps/Makefile
sed 's#JEMALLOC_CFLAGS= -std=gnu99 -Wall -pipe -g3 -O3#JEMALLOC_CFLAGS= -std=gnu99 -Wall -pipe -g3 -O0#' deps/Makefile > temp.txt
mv temp.txt deps/Makefile
make
cp src/redis-server ../binaries/redis/0
cd ..
rm -rf redis-5.0.7/

tar -xvf ../zips/redis-5.0.7.tar.gz
cd redis-5.0.7
export CFLAGS="-O1"
sed 's#OPTIMIZATION?=-O2#OPTIMIZATION?=-O1#' src/Makefile > temp.txt
mv temp.txt src/Makefile
sed 's#OPTIMIZATION?=-O3#OPTIMIZATION?=-O1#' deps/hiredis/Makefile > temp.txt
mv temp.txt deps/hiredis/Makefile
sed 's#OPT= -Os#OPT= -O1#' deps/linenoise/Makefile > temp.txt
mv temp.txt deps/linenoise/Makefile
sed 's#LUA_CFLAGS+= -O2#LUA_CFLAGS+= -O1#' deps/Makefile > temp.txt
mv temp.txt deps/Makefile
sed 's#JEMALLOC_CFLAGS= -std=gnu99 -Wall -pipe -g3 -O3#JEMALLOC_CFLAGS= -std=gnu99 -Wall -pipe -g3 -O1#' deps/Makefile > temp.txt
mv temp.txt deps/Makefile
make
cp src/redis-server ../binaries/redis/1
cd ..
rm -rf redis-5.0.7/

tar -xvf ../zips/redis-5.0.7.tar.gz
cd redis-5.0.7
export CFLAGS="-O2"
sed 's#OPTIMIZATION?=-O2#OPTIMIZATION?=-O2#' src/Makefile > temp.txt
mv temp.txt src/Makefile
sed 's#OPTIMIZATION?=-O3#OPTIMIZATION?=-O2#' deps/hiredis/Makefile > temp.txt
mv temp.txt deps/hiredis/Makefile
sed 's#OPT= -Os#OPT= -O2#' deps/linenoise/Makefile > temp.txt
mv temp.txt deps/linenoise/Makefile
sed 's#LUA_CFLAGS+= -O2#LUA_CFLAGS+= -O2#' deps/Makefile > temp.txt
mv temp.txt deps/Makefile
sed 's#JEMALLOC_CFLAGS= -std=gnu99 -Wall -pipe -g3 -O3#JEMALLOC_CFLAGS= -std=gnu99 -Wall -pipe -g3 -O2#' deps/Makefile > temp.txt
mv temp.txt deps/Makefile
make
cp src/redis-server ../binaries/redis/2
cd ..
rm -rf redis-5.0.7/

tar -xvf ../zips/redis-5.0.7.tar.gz
cd redis-5.0.7
export CFLAGS="-O3"
sed 's#OPTIMIZATION?=-O2#OPTIMIZATION?=-O3#' src/Makefile > temp.txt
mv temp.txt src/Makefile
sed 's#OPTIMIZATION?=-O3#OPTIMIZATION?=-O3#' deps/hiredis/Makefile > temp.txt
mv temp.txt deps/hiredis/Makefile
sed 's#OPT= -Os#OPT= -O3#' deps/linenoise/Makefile > temp.txt
mv temp.txt deps/linenoise/Makefile
sed 's#LUA_CFLAGS+= -O2#LUA_CFLAGS+= -O3#' deps/Makefile > temp.txt
mv temp.txt deps/Makefile
sed 's#JEMALLOC_CFLAGS= -std=gnu99 -Wall -pipe -g3 -O3#JEMALLOC_CFLAGS= -std=gnu99 -Wall -pipe -g3 -O3#' deps/Makefile > temp.txt
mv temp.txt deps/Makefile
make
cp src/redis-server ../binaries/redis/3
cd ..
rm -rf redis-5.0.7/


