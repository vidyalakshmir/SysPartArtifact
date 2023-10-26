sudo dpkg -i build_files/bind/libcap2-dbgsym_2.25-1.2_amd64.ddeb 
sudo groupadd bind
sudo useradd bind -g bind -s /sbin/nologin -M
tar -xvf libuv-1.34.0.tar.gz
cd libuv-1.34.0
sh autogen.sh 
LIBUV_DIR=$(pwd)
CURDIR1=$LIBUV_DIR/libuv-install
mkdir $CURDIR1
./configure --prefix=$CURDIR1
make
make check
make install
cd ../

tar -xvf bind9.tar.gz
cd bind9/
CURDIR2=$(pwd)/bind-install
mkdir $CURDIR2
export PKG_CONFIG_PATH=$LIBUV_DIR
LDFLAGS=-Wl,-rpath,$LIBUV_DIR/libuv-install/lib ./configure --without-libxml2 --prefix=$CURDIR2 
make -j 8
make install
cp ../build_files/bind/etc/* $CURDIR2/etc/
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


