export OUT=/home/syspart/SysPartArtifact/outputs
export PIN_ROOT=/home/syspart/SysPartArtifact/analysis/tools/pin-3.11-97998-g7ecce2dac-gcc-linux
cd src/pintool
echo ""
echo "Running dominant loop analysis on"
echo "REDIS ....."
echo "Press Ctrl+C after 10 seconds to exit"
sleep 2
mkdir -p $OUT/redis/pin
rm -rf $OUT/redis/pin/*
rm -f $OUT/redis/pin.out
sudo $PIN_ROOT/pin -follow_execv -t obj-intel64/timeofouterloop.so -i $OUT/redis/loops.out -p $OUT/redis/pin/ -o pin.out -- $REDIS
sleep 5
../scripts/parse_pinout.sh $OUT/redis/pin
echo ""
echo ""
echo "The main loops of redis are : "
cat $OUT/redis/pin.out
