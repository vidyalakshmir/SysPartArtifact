# SysPart Enforcement Component

This component creates a seccomp-BPF filter program with function `install_filter()`, which enables only the set of system calls identified in the analysis phase. The seccomp-BPF filter program is compiled into a shared library `sysfilter.so`. Egalito is used to insert a call to `insert_filter()` at the partition boundary identified during the analysis phase. Egalito then transforms the original binary to create a new binary with the system call filter.

_Note: Only one active filter at a time is currently supported._


## Requirements

1. Python 3 (Tested with Python 3.6.9)
2. Egalito

## Building

```
cd enforcement
make
```


## Filter Generation and Installation


### Step 1 - Setup

Set up the following environment variables (`$OUT` is the same folder used by the analysis component):

- `export OUT=<SysPart_outputs_directory>`

We use the same running example (Redis) with the analysis component.

### Step 2 - Generate Filter

Generate the Seccomp BPF Filter from the system calls found in the analysis phase.

```
cd enforcement/src/scripts
python3 skip_list_filter.py <syscalls_json_file> > ../filter/filter.c
cd ../filter
make
```

Example:

```
cd enforcement/src/scripts
python3 skip_list_filter.py $OUT/redis/syscalls1.json > ../filter/filter.c
cd ../filter
make
```

This creates `sysfilter.so in the enforcement/ directory.



### Step 3 - Insert Filter in Binary

Transform the binary by inserting the system call filter at the partition boundary.

```
cd enforcement
./sysenforce <binary> <func_name> <partition_addr> <output_binary> sysfilter.so install_filter
```

(`func_name`, `partition_addr`) pairs are from `$OUT/pin.out` and should be the same used to produce the json file containing the allowed system calls, produced by the analysis component.

Example:

```
./sysenforce <redis_dir>/redis-server-alt aeMain 45f30 new_redis sysfilter.so install_filter
```
