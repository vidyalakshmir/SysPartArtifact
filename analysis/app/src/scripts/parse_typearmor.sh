#!/bin/bash

curdir=$1
newfile="${curdir}/typearmor_parsed.txt" 
rm -f $newfile
for myfile in ${curdir}/binfo.*
	do
	myfilename=$(basename ${myfile})
	lib=${myfilename#*.}

	status="start"
	while IFS= read -r line
	do
		a=( $line )
		if [[ $line == "" ]]
		then
			continue
		fi
		if [[ ${a[0]} == "[varargs]" || ${a[0]} == "[args]" ]]
		then
			status="function"
			continue
		elif [[ ${a[0]} == "[icall-args]" ]]
		then
			status="icall"
			continue
		elif [[ ${a[0]} == "[non-voids]" ]]
		then
			status="voidfun"
			continue
		elif [[ ${a[0]} == "[non-void-icalls]" ]]
		then
			status="voidicall"
			continue
		elif [[ ${a[0]} == "[plts]" || ${a[0]} == "[disas-errors]" || ${a[0]} == "[prof-goals]" || ${a[0]} == "[unused]" || ${a[0]} == "[done]" ]]
		then
			status="unknown"
			continue
		fi
		if [[ $status == "function" ]]
		then
			echo "fnargs ${lib} ${a[0]} ${a[2]}" >> $newfile
		elif [[ $status == "icall" ]]
		then	
			echo "icallargs ${lib} ${a[0]} ${a[2]}" >> $newfile
		elif [[ $status == "voidfun" ]]
		then
			echo "nonvoidfn ${lib} ${a[0]}" >> $newfile
		elif [[ $status == "voidicall" ]]
		then
			echo "nonvoidicall ${lib} ${a[0]}" >> $newfile
		fi
	done < "$myfile"
done
