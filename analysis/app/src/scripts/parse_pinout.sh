#!/bin/bash

indir=$1
#outdir=$1/../
outdir="$(dirname "$indir")"
multiprocess=0
multithread=0
i=0
for f in "$indir"/*
do
	#echo "$f"
	fname=$(basename $f)
	tid=$(echo $fname | cut -d_ -f1)
	pid=$(echo $fname | cut -d_ -f2)
	if [ $i != 0 ]; then
		if [ $ppid != $pid ]; then
			multiprocess=1
		elif [ $ttid != $tid ]; then
			multithread=1
		fi
	fi
	out1=$(sort -r -k 1,1 $f | head -1 | awk {'print $4 "\t" $3'})
	offset=$(sort -r -k 1,1 $f | head -1 | awk {'print $3'})
	funcname=$(sort -r -k 1,1 $f | head -1 | awk {'print $4'})
	startaddr=$(grep -w "$funcname" $outdir/functions.out | awk {'print $3'})
	out=$(printf "($funcname+0x%x)\n" $((16#$offset - 16#$startaddr)))
	echo "$out1 $out" >> $outdir/pin.out
	ppid=$pid
	ttid=$tid
	((i=i+1))	
done

echo " "
echo "***** OUTPUT ********"
echo "Output written to $outdir/pin.out"
echo " "
if [ $multiprocess == 0 ] && [ $multithread == 0 ]; then
	echo "The server is single process and single-threaded" >> $outdir/pin.out
elif [ $multiprocess == 1 ] && [ $multithread == 1 ]; then
	echo "The server is multi-rocess and multi-threaded"
elif [ $multiprocess == 0 ] && [ $multithread == 1 ]; then
	echo "The server is single process and multi-threaded"
elif [ $multiprocess == 1 ] && [ $multithread == 0 ]; then
	echo "The server is multi-process and single-threaded"
fi
