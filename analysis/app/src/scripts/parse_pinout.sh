#!/bin/bash

indir=$1
outdir=$1/../

for f in "$indir"/*
do
	echo "$f"
	sort -r -k 1,1 $f | head -1 | awk {'print $4 "\t" $3'} >> $outdir/pin.out
done