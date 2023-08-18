# SysPart Analysis Component


The analysis phase consists of two parts:

##### a) Finding the initialization-main loop partition boundary (Steps 1-4).

The transition from the initialization to the serving phase of the server (which contains the server main loop) is identified in this phase. The algorithm goes on to find the top-level loop that executes for the most time (this is the case for the server main loops).

##### b) Computing the system calls used by different partitions and generating the system-call filter that should be inserted in the partition boundary (Steps 5-6).




## Tested on Linux

- x86-64
- Ubuntu 18.04.5 LTS
- gcc version 7.5.0
- 8GB RAM


## Configuring and Building

### Egalito

Instructions to build Egalito can be found in `analysis/tools/egalito/README` or you can try the following:

```
cd analysis/tools/egalito
sudo apt-get install make g++ libreadline-dev gdb lsb-release unzip
sudo apt-get install libc6-dbg libstdc++6-7-dbg  # names may differ
make -j `nproc`
```

You can refer to https://egalito.org/docs/tutorial.html for more information

#### Intel's Pin

Pin is a dynamic binary instrumentation framework that enables the creation of dynamic program analysis tools. (https://software.intel.com/content/www/us/en/develop/articles/pin-a-dynamic-binary-instrumentation-tool.html)

Download and install the latest 3.x version of Pin from: https://software.intel.com/content/www/us/en/develop/articles/pin-a-binary-instrumentation-tool-downloads.html

SysPart has been tested with Pin v3.11.


### TypeArmor (Optional)

TypeArmor is a static analysis tool for binaries that is used to retrieve the argument count information for callees and callsites of compiled C/C++ applications. TypeArmor is used by SysPart to improve the callgraph. It is optional, but recommended, since its use leads to better system-call filter. 

Cloning the TypeArmor repository: `git clone https://github.com/vusec/typearmor.git`

Instructions for configuring and building TypeArmor can be found in `typearmor/README.md`. Among other TypeArmor requires Dyninst.


### SysPart

Build SysPart static analysis tool. 

```
cd analysis/app
make
```

Build SysPart dynamic analysis tool. 

```
cd src/pintool
export PIN_ROOT=<pin_directory>
make
```


## Running


### Step1 - Setup 

Create an output directory for SysPart to store the output of each application. For example, if the application is redis, create `./outputs/redis`. 

Set up the following environment variables:

- `export OUT=<app_output_directory>`
- `export PIN_ROOT=<pin_directory>`

Build or download the binary you want to apply SysPart on. The binary and its library dependencies should include relocation information. Most recent distributions include position independent binaries that also include the necessary relocations.

While symbols are not required for every library on the system, we highly recommend using them when available. Symbols **are necessary** in the main binary for this prototype to work. Symbols for system libraries, like libc, can be installed with the following: 

`sudo apt-get install libc6-dbg libstdc++6-7-dbg`

#### Install target binary

All binaries that we used for evaluation are within the folder Artifact/binaries.

export BIND = binaries/bind9/install/sbin/named
export HTTPD = binaries/httpd-2.4.39/install/bin/httpd
export LIGHTTPD = binaries/lighttpd-1.4.54/install/sbin/lighttpd
export MEMCACHED = binaries/memcached/sasl-install/bin/memcached
export NGINX =	binaries/nginx-1.17.1/install/sbin/nginx
export REDIS = 	binaries/redis-5.0.7/src/redis-server


To run the binaries :

sudo $BIND -f -u bind

sudo $HTTPD

sudo $LIGHTTPD -D -f config/lighttpd.conf

$MEMCACHED -S -m 64 -p 11211 -u memcache -l 127.0.0.1

sudo  $NGINX

$REDIS binaries/redis-5.0.7/redis.conf


##### Install symbol packages for libraries (when available)

Example:

```
$ldd src/redis-server-alt

    /lib/x86_64-linux-gnu/librt.so.1 (0x00007f6a1d9f9000)
	libssl.so.1.1 => /usr/lib/x86_64-linux-gnu/libssl.so.1.1 (0x00007f6a1d967000)
	libcrypto.so.1.1 => /usr/lib/x86_64-linux-gnu/libcrypto.so.1.1 (0x00007f6a1d67e000)
	libpthread.so.0 => /lib/x86_64-linux-gnu/libpthread.so.0 (0x00007f6a1d65d000)
	libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f6a1d49a000)
	/lib64/ld-linux-x86-64.so.2 (0x00007f6a1dfe2000)
```

One library has a debug package.

`sudo apt-get install libcrypto++6-dbg libssl1.1-dbgsym`



### Step 2 - Loops Extraction

In this step we use Egalito to find all loops of the binary.

`./analysis/app/loops <binary> > $OUT/loops.out`

Example:
`./analysis/app/loops <redis_dir>/src/redis-server-alt > $OUT/loops.out`

### Step 3 - Find Dominant Loops

In this step, we perform dynamic analysis using Pin to find the partition boundary (main loop of the server). In this phase, the binary is run over Pin and our analysis tool, passing `loops.out` obtained in Step 2 as input. It computes the outermost loop that dominates execution (i.e., executes for most time) per process and per thread.

```
mkdir $OUT/pin
cd analysis/app/src/pintool
$PIN_ROOT/pin -follow_execv -t obj-intel64/timeofouterloop.so -i $OUT/loops.out -p $OUT/pin/ -- <command_line_executing_binary>
```

_Note : Make sure to put a '/' at the end of the directory name -p `$OUT/pin/`_

In most cases, it is sufficient to terminate the server after a minute. Sending some requests to the server may be necessary, if the main loop is not entered unless requests are sent.

Example:

```
$PIN_ROOT/pin -follow_execv -t obj-intel64/timeofouterloop.so -i $OUT/loops.out -p $OUT/pin/ -- <redis_dir>/src/redis-server-alt
```

You can terminate redis server after a minute or so, using Ctrl-C.

Each of the servers can be run using the following commands :


### Step 4 - Combine Analyses to Define Main Loop(s)

This script parses the pin output files and creates a output file `$OUT/pin.out`, which contains the function names and addresses of the partitions (server mainloops).

```
./analysis/app/src/scripts/parse_pinout.sh $OUT/pin
```

### (Optional) Step 5 - Use TypeArmor with Binary

#### Process binary

Process the binary using TypeArmor. To do this first copy the binary into TypeArmor's `server-bins` directory and then process it with TypeArmor.

Example:

```
cd (typearmor_dir)/server_bins/
cp <redis_dir>/redis-server-alt .
../run-ta-static.sh ./redis-server-alt
```

Copy the results of TypeArmor in the `$OUT` directory.

Example:

`cp -r <typarmor_dir>/out $OUT/typearmor`

#### Parse Results

```
cd <SysPart_dir>
./analysis/app/src/scripts/parse_typearmor.sh $OUT/typearmor
```

This will produce `$OUT/typearmor/typearmor_parsed.out`.


### Step 6 - Compute the Set of System Calls Used by the Dominant Loop Partitions


**Without** TypeArmor data:

```
./syspart -p <binary> -i -s main -a 2,<partition_addr>,<func_name> > $OUT/syscalls.json
```

**With** TypeArmor data:

Add `-t $OUT/typearmor/typearmor_parsed.out`

```
./syspart -p <binary> -i -s main -a 2,<partition_addr>,<func_name> -t $OUT/typearmor/typearmor_parsed.out > $OUT/syscalls.json
```

(`func_name`, `partition_addr`) pairs are obtained from `$OUT/pin.out`. _Note: There can be multiple pairs, because the output is per process/thread. To get the system calls of each pair, the above command has to be run separately for each pair._


Example:

Obtaining the pairs from `pin.out`.

```
$ cat $OUT/pin.out | sort | uniq
aeMain	45f30
bioProcessBackgroundJobs	b3bcf
background_thread_entry	13152c
```
Output of table 1 is generated by this step and all serving phases of the server be seen in pin.out

Generating the system call lists.This will generate Mainloop numbers for SysPart in table 2.

```
cd analysis/app
mkdir -p $OUT/redis
./syspart -p <redis_dir>/src/redis-server-alt -i -s main -a 2,45f30,aeMain > $OUT/redis/syscalls1.json
./syspart -p <redis_dir>/src/redis-server-alt -i -s main -a 2,b3bcf,bioProcessBackgroundJobs > $OUT/redis/syscalls2.json
./syspart -p <redis_dir>/src/redis-server-alt -i -s main -a 2,13152c,background_thread_entry > $OUT/redis/syscalls3.json
```

To get the numbers corresponding to main() for SysPart in table 2,
./syspart -p <redis_dir>/src/redis-server-alt -i -s main -a 7,main

Results of table 8 can be obtained by running the above commands. The last line of output will be execution times.


`./syspart --help` will show the other analyses that can be performed with the tool.


## Troubleshooting

### (Egalito) No struct type named rtld_global

The error `No struct type named rtld_global` may occur when running `make` in analyis/tools/egalito.
Make sure to have libc and libstdc++ debug packages installed (e.g. `libc6-dbg` and `libstdc++6-7-dbg`).
If the error still persists after having installed the debug packages, it is probably gdb
that is not able to read the debug symbols for the loaded libraries.
A workaround is to put the libraries with the debug symbols in the folder that gdb checks:

```
$ sudo cp /usr/lib/debug/lib/x86_64-linux-gnu/*.so /usr/lib/debug/usr/lib/x86_64-linux-gnu/
```

### (TypeArmor / Dyninst) LibElf error

If you forget to install the libelf package, Dyninst will try to install the LibElf library and may throw errors.
Solution is to install the libelf package and restart the building of Dyninst.

### (TypeArmor / Dyninst) undefined reference to boost symbols

We found this to happen when Dyninst cannot find the boost libraries on the machine.
The solution would be to tell Dyninst explicitly where they are. To find them you can execute

```
$ whereis libboost_system.so # in our case it is at /usr/lib/x86_64-linux-gnu
```

Then the cmake configuration command in the _dyninst-9.3.1/install_ folder would be
```
cmake .. -DCMAKE_INSTALL_PREFIX=`pwd` -DBoost_LIBRARY_DIR=/usr/lib/x86_64-linux-gnu
```

### (TypeArmor / Dyninst) cannot convert boost tribool to bool in return

The fix for this is to encapsulate the variable causing the error with `boot{..}`.

We encountered the error twice:

- Line 389 in dyninst-9.3.1/dataflowAPI/rose/util/Message.h: `!bold` -> `!bool{bold}`
- Line 1126 in dyninst-9.3.1/dataflowAPI/rose/util/Message.C: `!bi->second.isBuffered` -> `!bool{bi->second.isBuffered}`

### (TypeArmor) No such file: build\_envsetup.sh

To set up the environment execute `source envsetup.sh` after updating the path of Dyninst in envsetup.sh

### (TypeArmor) run-ta-static.sh: Segmentation fault

We found this to happen in two cases. One case is when `out` directory in the `typearmor` repository is missing
and the other case is a `NULL` dereference in _typearmor/static/arms_liveness.C_.
Make sure to create the `out` directory before running the `run-ta-static.sh` script.
To fix the `NULL` dereference apply the following fix to _typearmor/static/arms_livencess.C_:

```
diff --git a/static/arms_liveness.C b/static/arms_liveness.C
index a9dfdc8..e7ae970 100644
--- a/static/arms_liveness.C
+++ b/static/arms_liveness.C
@@ -2455,6 +2455,7 @@ int ArmsLiveness::get_icallsites(ArmsFunction *f) {
                       it != dependencies.end();
                       it++) {
                 ArmsFunction *dep = *it;
+                if (!dep) continue;
                 dprintf("[bt] icall %s.%d (%p) may benefit from profiling (now: %d args): lone (possible AT) function: %s (%p)\n", f->get_name().c_str(), i, (void*)block->get_last_insn_address(), max_arguments, dep->get_name().c_str(), (void *)dep->get_base_addr());
                 block->add_dependency(dep);
             }
```
