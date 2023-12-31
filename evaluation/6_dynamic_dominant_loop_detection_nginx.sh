cd ../analysis/app

base_dir=$(dirname $(dirname $(pwd)))
OUT="$base_dir/outputs"
export PIN_ROOT="$base_dir/analysis/tools/pin-3.11-97998-g7ecce2dac-gcc-linux"

cd src/pintool
make
echo ""
echo "Running dominant loop analysis on"
echo "NGINX ....."
mkdir -p $OUT/nginx/pin
rm -rf $OUT/nginx/pin/*
rm -f $OUT/nginx/pin.out
sudo $PIN_ROOT/pin -follow_execv -t obj-intel64/timeofouterloop.so -i $OUT/nginx/loops.out -p $OUT/nginx/pin/ -o pin.out -- $NGINX
sleep 5
sudo $NGINX -s quit
sleep 5
../scripts/parse_pinout.sh $OUT/nginx/pin
echo ""
echo ""
echo "The main loops of nginx are : "
cat $OUT/nginx/pin.out
