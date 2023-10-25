#!/bin/bash

# Assuming your file is named "input.txt"
input_file=$1
sf_folders=(
"/home/syspart/SysPartArtifact/related_work/sf/bind/"
"/home/syspart/SysPartArtifact/related_work/sf/httpd"
"/home/syspart/SysPartArtifact/related_work/sf/lighttpd"
"/home/syspart/SysPartArtifact/related_work/sf/memcached"
"/home/syspart/SysPartArtifact/related_work/sf/nginx"
"/home/syspart/SysPartArtifact/related_work/sf/redis"
)

sp_folders=(
"/home/syspart/SysPartArtifact/outputs/bind/syscall_results"
"/home/syspart/SysPartArtifact/outputs/httpd/syscall_results"
"/home/syspart/SysPartArtifact/outputs/lighttpd/syscall_results"
"/home/syspart/SysPartArtifact/outputs/memcached/syscall_results"
"/home/syspart/SysPartArtifact/outputs/nginx/syscall_results"
"/home/syspart/SysPartArtifact/outputs/redis/syscall_results"
)

tsp_folders=(
"/home/syspart/SysPartArtifact/related_work/tsp/bind"
"/home/syspart/SysPartArtifact/related_work/tsp/httpd"
"/home/syspart/SysPartArtifact/related_work/tsp/lighttpd"
"/home/syspart/SysPartArtifact/related_work/tsp/memcached"
"/home/syspart/SysPartArtifact/related_work/tsp/nginx"
"/home/syspart/SysPartArtifact/related_work/tsp/redis"
)
echo " "
echo "Results of Table 3 from paper"
echo " "
echo " "
echo "CVE		SystemCalls	SF	SP	TSP/TSP_fixed"
echo "-----------------------------------------------------------------"
# Loop through each line in the file
while IFS= read -r line
do
    # Split the line into fields using space as delimiter
    fields=($line)

    # The first field (fields[0]) is the CVE, the rest are system calls
    cve="${fields[0]}"
    system_calls=("${fields[@]:1}")
    num_system_calls=${#system_calls[@]}
    # Print the CVE and the associated system calls
    #echo "CVE: $cve"
    #echo "System Calls: ${system_calls[@]}"
    #echo "-----------------"
    sf_count=0
    for folder in "${sf_folders[@]}"   #for each server
    do
	i=0
    	for call in "${system_calls[@]}"
    	do
		if grep -q "$call" "$folder/allowed_syscalls.out"; then
			((i++))
		fi
    		
	done
	#echo "i is $i"
	if [ "$i" -eq 0 ]; then #this means that this server filtered all system calls and hence blocks this cve
		((sf_count++))
	fi

    done

       sp_count=0
    for folder in "${sp_folders[@]}"   #for each server
    do
        i=0
        for call in "${system_calls[@]}"
        do
                if grep -q "$call" "$folder/mainloop_sysnames.txt"; then
			#echo "$call $folder"
                        ((i++))
                fi

        done
        #echo "i is $i"
        if [ "$i" -eq 0 ]; then #this means that this server filtered all system calls and hence blocks this cve
                ((sp_count++))
        fi

    done

    tsp_count=0
    for folder in "${tsp_folders[@]}"   #for each server
    do
        i=0
        for call in "${system_calls[@]}"
        do
                if grep -q "$call" "$folder/mainloop.txt"; then
                        #echo "$call $folder"
                        ((i++))
                fi

        done
        #echo "i is $i"
        if [ "$i" -eq 0 ]; then #this means that this server filtered all system calls and hence blocks this cve
                ((tsp_count++))
        fi

    done

    fixed_tsp_count=0
    for folder in "${tsp_folders[@]}"   #for each server
    do
        i=0
        for call in "${system_calls[@]}"
        do
                if grep -q "$call" "$folder/fixed_mainloop.txt"; then
                        #echo "$call $folder"
                        ((i++))
                fi

        done
        #echo "i is $i"
        if [ "$i" -eq 0 ]; then #this means that this server filtered all system calls and hence blocks this cve
                ((fixed_tsp_count++))
        fi

    done


    echo "$cve ${system_calls[@]} $sf_count $sp_count $tsp_count/$fixed_tsp_count"


done < "$input_file"

