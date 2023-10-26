tar -xvf libevent-2.1.11-stable.tar.gz
cd libevent-2.1.11-stable
LIBEVENT_DIR=$(pwd)
CURDIR1=$LIBEVENT_DIR/libevent-install
mkdir $CURDIR1
./configure --prefix=$CURDIR1
make
make install


cd ../
tar -xvf memcached.tar.gz
cd memcached
CURDIR2=$(pwd)/memcached-install
mkdir $CURDIR2
./configure --prefix=$CURDIR2 --with-libevent=$CURDIR1 --enable-sasl
make
make install
cp $CURDIR2/bin/memcached ../final/
mkdir /etc/sasl2
echo "mech_list : plain" > /etc/sasl2/memcached.conf
echo "log_level : 5" >> /etc/sasl2/memcached.conf
