cd ../analysis/app
export OUT=/home/syspart/SysPartArtifact/outputs
export PIN_ROOT=/home/syspart/SysPartArtifact/analysis/tools/pin-3.11-97998-g7ecce2dac-gcc-linux
cd src/pintool
echo ""
echo "Running dominant loop analysis on"
echo "HTTPD ....."
mkdir -p $OUT/httpd/pin
rm -rf $OUT/httpd/pin/*
rm -f $OUT/httpd/pin.out
sudo $PIN_ROOT/pin -follow_execv -t obj-intel64/timeofouterloop.so -i $OUT/httpd/loops.out -p $OUT/httpd/pin/ -o pin.out -- $HTTPD
sleep 5
sudo $HTTPD -k graceful-stop
sleep 5
../scripts/parse_pinout.sh $OUT/httpd/pin
echo ""
echo ""
echo "The main loops of httpd are : "
cat $OUT/httpd/pin.out
echo ""

