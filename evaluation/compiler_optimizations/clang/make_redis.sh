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
sed 's#OPTIMIZATION?=-O2#OPTIMIZATION?=-O0#' src/Makefile > temp.txt
mv temp.txt src/Makefile
sed 's#OPTIMIZATION?=-O3#OPTIMIZATION?=-O0#' deps/hiredis/Makefile > temp.txt
mv temp.txt deps/hiredis/Makefile
sed 's#OPT= -Os#OPT= -O0#' deps/linenoise/Makefile > temp.txt
mv temp.txt deps/linenoise/Makefile
sed 's#LUA_CFLAGS+= -O2#LUA_CFLAGS+= -O0#' deps/Makefile > temp.txt
mv temp.txt deps/Makefile

sed 's#STLIB_MAKE_CMD=ar rcs $(STLIBNAME)#STLIB_MAKE_CMD=llvm-ar rcs $(STLIBNAME)#' deps/hiredis/Makefile > temp.txt
mv temp.txt deps/hiredis/Makefile

sed 's#AR= ar rcu#AR= llvm-ar rcu#' deps/lua/src/Makefile > temp.txt
mv temp.txt deps/lua/src/Makefile

sed 's#RANLIB= ranlib#RANLIB= llvm-ranlib#' deps/lua/src/Makefile > temp.txt
mv temp.txt deps/lua/src/Makefile

sed 's#AR=ar#AR=llvm-ar#' deps/Makefile > temp.txt
mv temp.txt deps/Makefile

sed 's#AR = ar#AR = llvm-ar#' deps/jemalloc/Makefile > temp.txt
mv temp.txt deps/jemalloc/Makefile

sed 's#RANLIB= ranlib#RANLIB= llvm-ranlib#' deps/lua/Makefile > temp.txt
mv temp.txt deps/lua/Makefile

sed 's;#define JEMALLOC_FRAG_HINT;//#define JEMALLOC_FRAG_HINT;' deps/jemalloc/include/jemalloc/jemalloc_macros.h > temp.txt
mv temp.txt deps/jemalloc/include/jemalloc/jemalloc_macros.h
sed 's;#define JEMALLOC_FRAG_HINT;//#define JEMALLOC_FRAG_HINT;' deps/jemalloc/include/jemalloc/jemalloc.h > temp.txt
mv temp.txt deps/jemalloc/include/jemalloc/jemalloc.h
sed 's;#define JEMALLOC_FRAG_HINT;//#define JEMALLOC_FRAG_HINT;' deps/jemalloc/include/jemalloc/jemalloc_macros.h.in > temp.txt
mv temp.txt deps/jemalloc/include/jemalloc/jemalloc_macros.h.in
sed 's#JEMALLOC_CFLAGS= -std=gnu99 -Wall -pipe -g3 -O3#JEMALLOC_CFLAGS= -std=gnu99 -Wall -pipe -g3 -O0#' deps/Makefile > temp.txt
mv temp.txt deps/Makefile
export CC=clang  
export AR=llvm-ar 
export LD=/usr/bin/ld 
export CFLAGS="-flto -O0 -fpic" 
export LDFLAGS="-flto -Wl -pie" 

make
cp src/redis-server ../binaries/redis/0
cd ..
rm -rf redis-5.0.7/

tar -xvf ../zips/redis-5.0.7.tar.gz
cd redis-5.0.7
sed 's#OPTIMIZATION?=-O2#OPTIMIZATION?=-O1#' src/Makefile > temp.txt
mv temp.txt src/Makefile
sed 's#OPTIMIZATION?=-O3#OPTIMIZATION?=-O1#' deps/hiredis/Makefile > temp.txt
mv temp.txt deps/hiredis/Makefile
sed 's#OPT= -Os#OPT= -O1#' deps/linenoise/Makefile > temp.txt
mv temp.txt deps/linenoise/Makefile
sed 's#LUA_CFLAGS+= -O2#LUA_CFLAGS+= -O1#' deps/Makefile > temp.txt
mv temp.txt deps/Makefile

sed 's#STLIB_MAKE_CMD=ar rcs $(STLIBNAME)#STLIB_MAKE_CMD=llvm-ar rcs $(STLIBNAME)#' deps/hiredis/Makefile > temp.txt
mv temp.txt deps/hiredis/Makefile

sed 's#AR= ar rcu#AR= llvm-ar rcu#' deps/lua/src/Makefile > temp.txt
mv temp.txt deps/lua/src/Makefile

sed 's#RANLIB= ranlib#RANLIB= llvm-ranlib#' deps/lua/src/Makefile > temp.txt
mv temp.txt deps/lua/src/Makefile

sed 's#AR=ar#AR=llvm-ar#' deps/Makefile > temp.txt
mv temp.txt deps/Makefile

sed 's#AR = ar#AR = llvm-ar#' deps/jemalloc/Makefile > temp.txt
mv temp.txt deps/jemalloc/Makefile

sed 's#RANLIB= ranlib#RANLIB= llvm-ranlib#' deps/lua/Makefile > temp.txt
mv temp.txt deps/lua/Makefile

sed 's;#define JEMALLOC_FRAG_HINT;//#define JEMALLOC_FRAG_HINT;' deps/jemalloc/include/jemalloc/jemalloc_macros.h > temp.txt
mv temp.txt deps/jemalloc/include/jemalloc/jemalloc_macros.h
sed 's;#define JEMALLOC_FRAG_HINT;//#define JEMALLOC_FRAG_HINT;' deps/jemalloc/include/jemalloc/jemalloc.h > temp.txt
mv temp.txt deps/jemalloc/include/jemalloc/jemalloc.h
sed 's;#define JEMALLOC_FRAG_HINT;//#define JEMALLOC_FRAG_HINT;' deps/jemalloc/include/jemalloc/jemalloc_macros.h.in > temp.txt
mv temp.txt deps/jemalloc/include/jemalloc/jemalloc_macros.h.in
sed 's#JEMALLOC_CFLAGS= -std=gnu99 -Wall -pipe -g3 -O3#JEMALLOC_CFLAGS= -std=gnu99 -Wall -pipe -g3 -O1#' deps/Makefile > temp.txt
mv temp.txt deps/Makefile
export CC=clang  
export AR=llvm-ar 
export LD=/usr/bin/ld 
export CFLAGS="-flto -O1 -fpic" 
export LDFLAGS="-flto -Wl -pie" 

make
cp src/redis-server ../binaries/redis/1
cd ..
rm -rf redis-5.0.7/



tar -xvf ../zips/redis-5.0.7.tar.gz
cd redis-5.0.7
sed 's#OPTIMIZATION?=-O2#OPTIMIZATION?=-O2#' src/Makefile > temp.txt
mv temp.txt src/Makefile
sed 's#OPTIMIZATION?=-O3#OPTIMIZATION?=-O2#' deps/hiredis/Makefile > temp.txt
mv temp.txt deps/hiredis/Makefile
sed 's#OPT= -Os#OPT= -O2#' deps/linenoise/Makefile > temp.txt
mv temp.txt deps/linenoise/Makefile
sed 's#LUA_CFLAGS+= -O2#LUA_CFLAGS+= -O2#' deps/Makefile > temp.txt
mv temp.txt deps/Makefile

sed 's#STLIB_MAKE_CMD=ar rcs $(STLIBNAME)#STLIB_MAKE_CMD=llvm-ar rcs $(STLIBNAME)#' deps/hiredis/Makefile > temp.txt
mv temp.txt deps/hiredis/Makefile

sed 's#AR= ar rcu#AR= llvm-ar rcu#' deps/lua/src/Makefile > temp.txt
mv temp.txt deps/lua/src/Makefile

sed 's#RANLIB= ranlib#RANLIB= llvm-ranlib#' deps/lua/src/Makefile > temp.txt
mv temp.txt deps/lua/src/Makefile

sed 's#AR=ar#AR=llvm-ar#' deps/Makefile > temp.txt
mv temp.txt deps/Makefile

sed 's#AR = ar#AR = llvm-ar#' deps/jemalloc/Makefile > temp.txt
mv temp.txt deps/jemalloc/Makefile

sed 's#RANLIB= ranlib#RANLIB= llvm-ranlib#' deps/lua/Makefile > temp.txt
mv temp.txt deps/lua/Makefile

sed 's;#define JEMALLOC_FRAG_HINT;//#define JEMALLOC_FRAG_HINT;' deps/jemalloc/include/jemalloc/jemalloc_macros.h > temp.txt
mv temp.txt deps/jemalloc/include/jemalloc/jemalloc_macros.h
sed 's;#define JEMALLOC_FRAG_HINT;//#define JEMALLOC_FRAG_HINT;' deps/jemalloc/include/jemalloc/jemalloc.h > temp.txt
mv temp.txt deps/jemalloc/include/jemalloc/jemalloc.h
sed 's;#define JEMALLOC_FRAG_HINT;//#define JEMALLOC_FRAG_HINT;' deps/jemalloc/include/jemalloc/jemalloc_macros.h.in > temp.txt
mv temp.txt deps/jemalloc/include/jemalloc/jemalloc_macros.h.in
sed 's#JEMALLOC_CFLAGS= -std=gnu99 -Wall -pipe -g3 -O3#JEMALLOC_CFLAGS= -std=gnu99 -Wall -pipe -g3 -O2#' deps/Makefile > temp.txt
mv temp.txt deps/Makefile
export CC=clang  
export AR=llvm-ar 
export LD=/usr/bin/ld 
export CFLAGS="-flto -O2 -fpic" 
export LDFLAGS="-flto -Wl -pie" 

make
cp src/redis-server ../binaries/redis/2
cd ..
rm -rf redis-5.0.7/

tar -xvf ../zips/redis-5.0.7.tar.gz
cd redis-5.0.7
sed 's#OPTIMIZATION?=-O2#OPTIMIZATION?=-O3#' src/Makefile > temp.txt
mv temp.txt src/Makefile
sed 's#OPTIMIZATION?=-O3#OPTIMIZATION?=-O3#' deps/hiredis/Makefile > temp.txt
mv temp.txt deps/hiredis/Makefile
sed 's#OPT= -Os#OPT= -O3#' deps/linenoise/Makefile > temp.txt
mv temp.txt deps/linenoise/Makefile
sed 's#LUA_CFLAGS+= -O2#LUA_CFLAGS+= -O3#' deps/Makefile > temp.txt
mv temp.txt deps/Makefile

sed 's#STLIB_MAKE_CMD=ar rcs $(STLIBNAME)#STLIB_MAKE_CMD=llvm-ar rcs $(STLIBNAME)#' deps/hiredis/Makefile > temp.txt
mv temp.txt deps/hiredis/Makefile

sed 's#AR= ar rcu#AR= llvm-ar rcu#' deps/lua/src/Makefile > temp.txt
mv temp.txt deps/lua/src/Makefile

sed 's#RANLIB= ranlib#RANLIB= llvm-ranlib#' deps/lua/src/Makefile > temp.txt
mv temp.txt deps/lua/src/Makefile

sed 's#AR=ar#AR=llvm-ar#' deps/Makefile > temp.txt
mv temp.txt deps/Makefile

sed 's#AR = ar#AR = llvm-ar#' deps/jemalloc/Makefile > temp.txt
mv temp.txt deps/jemalloc/Makefile

sed 's#RANLIB= ranlib#RANLIB= llvm-ranlib#' deps/lua/Makefile > temp.txt
mv temp.txt deps/lua/Makefile

sed 's;#define JEMALLOC_FRAG_HINT;//#define JEMALLOC_FRAG_HINT;' deps/jemalloc/include/jemalloc/jemalloc_macros.h > temp.txt
mv temp.txt deps/jemalloc/include/jemalloc/jemalloc_macros.h
sed 's;#define JEMALLOC_FRAG_HINT;//#define JEMALLOC_FRAG_HINT;' deps/jemalloc/include/jemalloc/jemalloc.h > temp.txt
mv temp.txt deps/jemalloc/include/jemalloc/jemalloc.h
sed 's;#define JEMALLOC_FRAG_HINT;//#define JEMALLOC_FRAG_HINT;' deps/jemalloc/include/jemalloc/jemalloc_macros.h.in > temp.txt
mv temp.txt deps/jemalloc/include/jemalloc/jemalloc_macros.h.in
sed 's#JEMALLOC_CFLAGS= -std=gnu99 -Wall -pipe -g3 -O3#JEMALLOC_CFLAGS= -std=gnu99 -Wall -pipe -g3 -O3#' deps/Makefile > temp.txt
mv temp.txt deps/Makefile
export CC=clang  
export AR=llvm-ar 
export LD=/usr/bin/ld 
export CFLAGS="-flto -O3 -fpic" 
export LDFLAGS="-flto -Wl -pie" 

make
cp src/redis-server ../binaries/redis/3
cd ..
rm -rf redis-5.0.7/