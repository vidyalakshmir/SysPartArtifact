export OUT=/home/syspart/SysPartArtifact/outputs
export PIN_ROOT=/home/syspart/SysPartArtifact/analysis/tools/pin-3.11-97998-g7ecce2dac-gcc-linux
cd src/pintool
echo ""
echo "Running dominant loop analysis on"
echo "MEMCACHED ....."
mkdir -p $OUT/memcached/pin
rm -rf $OUT/memcached/pin/*
rm -f $OUT/memcached/pin.out
echo "Press Ctrl+C after 10 seconds to exit"
$PIN_ROOT/pin -follow_execv -t obj-intel64/timeofouterloop.so -i $OUT/memcached/loops.out -p $OUT/memcached/pin/ -o pin.out -- $MEMCACHED -S -m 64 -p 11211 -u memcache -l 127.0.0.1
../scripts/parse_pinout.sh $OUT/memcached/pin
echo ""
echo ""
echo "The main loops of memcached are : "
cat $OUT/memcached/pin.out

