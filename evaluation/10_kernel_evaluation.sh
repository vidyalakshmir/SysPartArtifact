#!/bin/bash

cd ../analysis/app
base_name=$(dirname $(dirname $(pwd)))
input_file="$base_name/evaluation/kernel_eval/kernel_cve.out"
sf_folders=(
"$base_name/related_work/sf/bind/"
"$base_name/related_work/sf/httpd"
"$base_name/related_work/sf/lighttpd"
"$base_name/related_work/sf/memcached"
"$base_name/related_work/sf/nginx"
"$base_name/related_work/sf/redis"
)

sp_folders=(
"$base_name/outputs/bind/syscall_results"
"$base_name/outputs/httpd/syscall_results"
"$base_name/outputs/lighttpd/syscall_results"
"$base_name/outputs/memcached/syscall_results"
"$base_name/outputs/nginx/syscall_results"
"$base_name/outputs/redis/syscall_results"
)

tsp_folders=(
"$base_name/related_work/tsp/bind"
"$base_name/related_work/tsp/httpd"
"$base_name/related_work/tsp/lighttpd"
"$base_name/related_work/tsp/memcached"
"$base_name/related_work/tsp/nginx"
"$base_name/related_work/tsp/redis"
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

