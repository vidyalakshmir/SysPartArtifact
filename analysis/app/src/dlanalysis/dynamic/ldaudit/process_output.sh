#!/bin/bash
#set -x
out=$1
cd output/
for FILE in *.txt
do
	line=$(grep -n 'la_preinit' $FILE | awk {'print $1'} | sed 's/:.*//')

	result=$(awk "NR>=$line" $FILE | grep -n 'la_objopen()' | sed 's/:.*://' | awk {'print $1 " " $3 " " $7'} | sed 's/"//'g | sed 's/;//'g | sed 's/cookie=//')

	i=1
	x=1
	y=2
	z=3
	for r in $result	
	do

		if [[ $i -eq $x ]]
		then 
			ln=$r
			i=$((i+1))
		elif [[ $i -eq $y ]]
		then
			lib=$r

			i=$((i+1))
		elif [[ $i -eq $z ]] 
		then 
			cookie=$r
			
			sym=$(awk "NR>=$line" $FILE | awk "NR>=$ln" | grep -m 1 -B1 "defcook = $cookie" | grep 'symname' | sed 's/.*symname =//'  | awk {'print $1'} | sed 's/;//'g)
			echo "$lib $sym" 
			i=1
		fi
	done

done
cd ..
mv output/* $1

