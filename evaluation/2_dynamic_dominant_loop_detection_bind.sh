cd ../analysis/app
base_dir=$(dirname $(dirname $(pwd)))
OUT="$base_dir/outputs"
PIN_ROOT="$base_dir/analysis/tools/pin-3.11-97998-g7ecce2dac-gcc-linux"

cd src/pintool
echo ""
echo "Running dominant loop analysis on"
echo "BIND ....."
mkdir -p $OUT/bind/pin
rm -rf $OUT/bind/pin/*
rm -f $OUT/bind/pin.out
echo "Press Ctrl+C after 10 seconds to exit"
sudo $PIN_ROOT/pin -follow_execv -t obj-intel64/timeofouterloop.so -i $OUT/bind/loops.out -p $OUT/bind/pin/ -o pin.out -- $BIND -f -u bind
../scripts/parse_pinout.sh $OUT/bind/pin
echo ""
echo ""
echo "The main loops of bind are : "
cat $OUT/bind/pin.out
echo ""
