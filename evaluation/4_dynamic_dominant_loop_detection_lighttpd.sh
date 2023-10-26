cd ../analysis/app
base_dir=$(dirname $(dirname $(pwd)))
OUT="$base_dir/outputs"
PIN_ROOT="$base_dir/analysis/tools/pin-3.11-97998-g7ecce2dac-gcc-linux"
cd src/pintool
echo ""
echo "Running dominant loop analysis on"
echo "LIGHTTPD ....."
mkdir -p $OUT/lighttpd/pin
rm -rf $OUT/lighttpd/pin/*
rm -f $OUT/lighttpd/pin.out
echo "Press Ctrl+C after 10 seconds to exit"
$PIN_ROOT/pin -follow_execv -t obj-intel64/timeofouterloop.so -i $OUT/lighttpd/loops.out -p $OUT/lighttpd/pin/ -o pin.out -- $LIGHTTPD -D -f /home/syspart/SysPartArtifact/binaries/lighttpd-1.4.54/lighttpd-install/config/lighttpd.conf
../scripts/parse_pinout.sh $OUT/lighttpd/pin
echo ""
echo ""
echo "The main loops of lighttpd are : "
cat $OUT/lighttpd/pin.out
echo ""

