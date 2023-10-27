#!/bin/bash
app=$1
script_dir=$(dirname "$0")
pathfile="$script_dir/../pathlist_$app.txt"
file1="$script_dir/../all_libs_$app.txt"
rm -f $file1
while IFS= read -r line
do
		sudo find $line | grep '\.so$' >> $file1
done < "$pathfile"



file2="$script_dir/../unique_libs_$app.txt"
rm -f $file2
declare -A fnames
i=0
while IFS= read -r line
do
	filename=$(basename $line 2> /dev/null)
	exitcode=$?
	if [ $exitcode == "1" ]
	then
		continue
	fi
	fnames[$filename]=$line
done < "$file1"

for key in "${!fnames[@]}"; 
	do echo "${fnames[$key]}" >> $file2; 
done


