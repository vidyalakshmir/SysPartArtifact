#!/bin/bash
f=$1
app=$2
script_dir=$(dirname "$0")

myfile="$script_dir/../unique_libs_$app.txt"

while IFS= read -r line
do
	a=( $line )
	state=${a[0]}
	if [ $state == "FUNC" ]
	then
		func=${a[3]}
		addr=${a[5]}
	elif [ $state == "TYPE" ]
	then
		type=${a[1]}
		if [ $type == "1" ]
		then
			
			sym=${a[11]}
			while IFS= read -r line
			do
				b=( $line )
				ln=$(readelf -s "$b" 2> /dev/null | grep -w "$sym" 2> /dev/null | grep -v 'UND' 2> /dev/null | wc -l)
				#echo "line $b $ln"
				if [ $ln -gt 0 ]
				then
					echo "$func $addr $sym $b"
				fi
			done < "$myfile"

		else
			echo "$func $addr UNKNOWN"
		fi
	fi
done < "$f"