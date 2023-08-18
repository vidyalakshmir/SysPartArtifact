#!/bin/bash
binaryfile=$1
echo $binaryfile
output=$(readelf -r "$binaryfile" | grep libc_start_main | awk '{print $1}' | sed 's/0*//')
oldifs="$IFS"
IFS=$'\n'
output1=( $(objdump -d "$binaryfile" | grep -B 1 $output) )
if [[ ${output1[0]} =~ "%rdi" ]]
then
   startaddr=$(echo "${output1[0]}" | grep -o -P '(?<=# ).*(?= <)')
   echo $startaddr
fi
