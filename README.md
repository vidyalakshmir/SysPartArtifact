# SYSPART

This software is the implementation of our [paper](https://github.com/vidyalakshmir/SysPartArtifact/blob/main/paper/SYSPART.pdf) "SYSPART: Automated Temporal System Call Filtering for Binaries", published in ACM Conference on Computer and Communications Security (CCS) 2023. (https://dl.acm.org/doi/10.1145/3576915.3623207).

This repository is aimed at reproducing the results of the paper and contains all code and scripts to run the experiments of the paper. We have created another repository [SysPartCode](https://github.com/vidyalakshmir/SysPartCode) with only the code, which can be used for running different functionalities or modules.

```
@inproceedings{syspart_ccs23,
author = {Rajagopalan, Vidya Lakshmi and Kleftogiorgos, Konstantinos and G\"{o}ktas, Enes and Xu, Jun and Portokalidis, Georgios},
title = {SysPart: Automated Temporal System Call Filtering for Binaries},
year = {2023},
booktitle = {Proceedings of the 2023 ACM SIGSAC Conference on Computer and Communications Security},
pages = {1979–1993},
}
```


## Table of Contents

- [Directory Structure](#directory-structure)
- [Supported Environment](#supported-environment)
- [Major Claims & Experiments](#major-claims-&-experiments)
- [Setup](#setup)
- [Experiments](#experiments)
- [License](#license)

## Directory Structure

- [`analysis`](./analysis):
  - [`app/src`](./analysis/app/src): Source code of SYSPART (static analysis component)
  - [`app/src/dlanalysis`](./analysis/app/src/dlanalysis): Source code for dynamic library analysis (static & dynamic)
  - [`app/src/pintool`](./analysis/app/src/pintool): Source code for serving phase detection (dynamic analysis)
  - [`app/src/scripts`](./analysis/app/src/scripts): Scripts required for processing
  - [`tools`](./analysis/tools):Source code of tools required to run SYSPART - Egalito and Pin 3.11

- [`binaries`](./binaries): The source code and build scripts for all benchmark servers
- [`docker`](./docker): The dockerfile to obtain a docker container to run the experiments
- [`enforcement`](./enforcement): Source code for enforcing syscall filter using Seccomp-BPF
- [`evaluation`](./evaluation): Scripts for running all experiments
- [`outputs`](./outputs): Outputs of experiments
- [`related_work`](./related_work): Outputs of related works used in experiments

## Supported Environment

### Hardware
The experiments were run on 4-core Intel Core i7 8550U 1.80GHz CPU with 16GB RAM and 30GB of swap space. Having a hardware setup with minimum these requirements is recommended to run the experiments.

### Software
The experiments were performed on Ubuntu 18.04.6 LTS (kernel version 5.4.0-150). t is recommended to use this version of Ubuntu to achieve the outputs described in the paper, as the results depend on the specific library versions included in this Ubuntu release. Tools including [Egalito](https://github.com/stevens-s3lab/egalito.git), [Intel Pin 3.11](https://www.intel.com/content/www/us/en/developer/articles/tool/pin-a-dynamic-binary-instrumentation-tool.html) and [TypeArmor](https://github.com/vusec/typearmor) were used.


## Major Claims \& Experiments

- **(C1):** SYSPART only allows 8.33% more syscalls than TSP.
This is demonstrated by experiment (E1) and (E2) whose results are illustrated in Table 2.
	- **(E1):** Serving Phase Detection using static and dynamic analysis
	- **(E2):** Generation of syscall list of main() and mainloop using static analysis

- **(C2):** SYSPART Filters as many security-critical syscalls as TSP in 88.23% of cases and outperforms TSP in 2% of cases. This is demonstrated by experiment (E5) whose results are illustrated in Table 6.
	- **(E5):** Evaluates effectiveness of SYSPART in filtering security-sensitive syscalls

- **(C3):** SYSPART is effective in thwarting exploit payloads, with a success rate of 53.83% - 78.5%. This is demonstrated by experiment (E4) whose results are illustrated in Table 5.
	- **(E4):**  Evaluates effectiveness of SYSPART in thwarting kernel vulnerabilities

- **(C4):** SYSPART has a success rate of 36.11% - 77.77% in blocking kernel vulnerabilities, for the ones tested, depending on the application
This is demonstrated by experiment (E3) whose results are illustrated in Table 3. 
	- **(E3):**  Evaluates effectiveness of SYSPART in mitigating kernel vulnerabilities

- **(E6):** Generation of syscall list with different compilers and optimizations

## Setup
To reproduce the paper's major claims, we have created a benchmarking environment used in the paper using docker. We strongly encourage to use this docker to run all experiments to reproduce the results.

### Setup from Docker Image

The docker is setup with the necessary environment which includes the operating system, required libraries, tools and the benchmark applications. 

#### Obtain docker image
##### Already built docker image
``` shell
$ sudo docker pull vidyalakshmir/syspart:1.1
```

##### Build docker image from [Dockerfile](https://github.com/vidyalakshmir/SysPartArtifact/blob/main/docker/Dockerfile) contained in this repo
``` shell
$ cd SysPartArtifact/docker
$ sudo docker build -t vidyalakshmir/syspart:1.1 .
```

#### Create docker container
``` shell
$ sudo docker run -it vidyalakshmir/syspart:1.1 /bin/bash
```
This will create a docker container with the docker image and you will get a bash terminal to run the experiments. 

#### Update git repo
```shell
$ cd /home/syspart/SysPartArtifact
$ git pull
```
This updates the git repo to the latest commit.


#### Notes on the docker container
- The git repo containing the source code and evaluation scripts is within the folder `/home/syspart/SysPartArtifact`. 
- The benchmark applications are installed in `/home/syspart/SysPartArtifact/binaries`. 
- External tools required to run SYSPART (Egalito and Intel Pin) are installed in /home/syspart/SysPartArtifact/tools. 
- Outputs of TypeArmor can be found in `/home/syspart/SysPartArtifact/outputs/$app/typearmor` where $app can be any of the benchmark applications.
- Outputs of related work sysfilter (SF) and Temporal Specialization (TSP) are found in `/home/syspart/SysPartArtifact/related_work/sf` and `/home/syspart/SysPartArtifact/related_work/tsp respectively`.

### Setup from Scratch

#### Setup a machine with Ubuntu 18.04 installed

#### Install all dependencies
```
$ sudo apt-get update
$ sudo apt-get install sudo make g++ libreadline-dev gdb lsb-release libc6-dbg libstdc++6-7-dbg bc libpcre3 libpcre3-dev libssl1.1 libssl-dev libbz2-dev libtool automake pkg-config python-ply libcap-dev zlib1g-dbg zlib1g-dev uuid-runtime uuid-dev libexpat1-dev libsasl2-2 libsasl2-dev git vim clang unzip
```
#### Cloning the repo

Note: If you are using the docker image, cloning this repository is not required.

This repository uses several git submodules for benchmarking. To clone this repository, you __must__ have SSH access configured for [Github](https://github.com). (Please refer this [link](https://docs.github.com/en/authentication/connecting-to-github-with-ssh/adding-a-new-ssh-key-to-your-github-account) for more information.)

Once you ensure your public  keys are configured, you an clone the repository recursively with:

```shell
$ git clone --recursive https://github.com/vidyalakshmir/SysPartArtifact.git.
```

#### Install the bechmark applications

```shell
$ cd SysPartArtifact/binaries
$ ./make_bind.sh 
$ ./make_httpd.sh 
$ ./make_lighttpd.sh 
$ ./make_memcached.sh 
$ ./make_nginx.sh 
$ ./make_redis.sh 
```

#### Setup environmental variables
```shell
$ cd SysPartArtifact/binaries
$ ./set_env.sh
```

#### Build SYSPART
```shell
$ cd SysPartArtifact/analysis/tools/egalito
$ make -j 8
$ cd SysPartArtifact/analysis/tools/app
$ make
```
This will first build Egalito which is a dependency for SYSPART and then build SYSPART.

#### Install Intel Pin
$ cd SysPartArtifact/analysis/tools
$ tar -xvf pin-3.11-97998-g7ecce2dac-gcc-linux.tar.gz

### Other tools and related work
The repo contains pre-computed outputs from the following external tools and related work which are used for comparison in our experiments. Only these outputs are required to run the experiments. Installation of these tools are not necessary.

1. TypeArmor : This is an optional FCG refining tool used by SYSPART. The output from TypeArmor is included within the folder `SysPartArtifact/outputs/$app/typearmor`, where $app is one of the benchmark applications. It can be installed from scratch from [its repo](https://github.com/vusec/typearmor).

2. sysfilter (SF) : There are experiments that compare against the sysfilter tool. The results after running the benchmark applications using the sysfilter tool can be found in `SysPartArtifact/related_work/sf`. Instructions to install sysfilter can be found [here](https:
//github.com/vidyalakshmir/SysPartArtifact/
blob/main/related_work/sf/README.md).

3. Temporal Specialization (TSP) : There are experiments that compare against the TSP tool. The results after running the benchmark applications using the TSP tool can be found in `SysPartArtifact/related_work/tsp`. Instructions to install TSP can be found [here](https:
//github.com/vidyalakshmir/SysPartArtifact/
blob/main/related_work/tsp/README.md).

## Experiments

All the experiments have to be run from the folder `SysPartArtifact/evaluation`. Please execute them in the order described here because since the later experiments depend on the results of previous experiments.

### (E1): Serving Phase Detection
- Compute time: 7 minutes

#### Description
This experiments involves running the static analysis to obtain loop information of binaries and its dependent libraries. Then, Intel Pin is used to detect all top-level loops that are observed dynamically by running the servers with the pintool. Later, a script is used to obtain the dominant loops (main loops) of the server.

#### Execution
Execute the following command within the folder `SysPartArtifact/evaluation` to obtain loop information of binaries and dependent libraries using static analysis.

``` shell
$ ./1_static_loop_analysis
```

Execute the following commands to determine the main loops of the servers. Except for httpd and nginx, all other servers require user intervention to stop them (which will be displayed on the screen).

``` shell
$ ./2_dynamic_dominant_loop_detection_bind.sh
$ ./3_dynamic_dominant_loop_detection_httpd.sh
$ ./4_dynamic_dominant_loop_detection_lighttpd.sh
$ ./5_dynamic_dominant_loop_detection_memcached.sh
$ ./6_dynamic_dominant_loop_detection_nginx.sh 
$ ./7_dynamic_dominant_loop_detection_redis.sh
```

#### Results
This step produces the list of all main loops of the servers. Also, it lists down if the server is multi-process and/or multi-threaded. The columns 'Concurrency' and 'Main loop' in Table 1 can be observed from this experiment results.

### (E2): System calls of main() and mainloop
- Compute time: 30 minutes

#### Description
This experiment involves computing the FCG and generating the system calls of main() and main loop.

#### Execution
The server binary along with the address and function containing the main loop is provided as input to the tool.

``` shell
$ ./8_generate_syscall_filter.sh
```

#### Results
All four columns under `SYSPART` in table 2 can be observed with this experiment. The list of system calls of TSP and SF which have already been run and computed and can be found in the folder `SysPartArtifact/related_work`. This experiment validates claim C1.

### (E3): Kernel CVE Evaluation
- Compute time: 3 minutes

#### Description
This experiment evaluates effectiveness of SYSPART in mitigating kernel vulnerabilities and also compares it with SF and TSP.

#### Execution

``` shell
$ ./9_generate_syscall_list.sh
$ ./10_kernel_evaluation.sh
```

#### Results
All values in T
able 3 can be observed with this experiment. This experiment validates claim C4.

### (E4): Exploit Code Mitigation
- Compute time: 10 minutes

#### Description
This experiment evalutes effective of SYSPART in thwarting exploit payloads and compares against it with SF and TSP.

#### Execution

``` shell
./11_exploit_mitigation.sh
```

#### Results
All values in Table 5 can be observed with this experiment. This experiment validates our claim C3.

### (E5): Security sensitive system calls
- Compute time: 2 minutes

#### Description
This experiment evaluates effectiveness of SYSPART in filtering security-sensitive system calls and compares against SF and TSP.

#### Execution

```shell
$ ./14_security_sensitive_syscalls.sh
```

#### Results
All values in Table 6 can be observed with this experiment. This experiment validates claim C2.

### (E6): Results with different compilers and optimizations
- Compute time: 60 minutes

#### Description
This experiment builds the benchmark server applications with gcc and clang and optimization levels. Later, the different binaries produced are analyzed with SYSPART to produce the list of allowed syscalls in the mainloop.

#### Execution

```shell
$ ./15_1_compiler_optimizations.sh
$ ./15_2_compiler_optimizations.sh
```
#### Results
All values in Table 8 and explained in section 5.6.1 can be observed in the results.


## License

This software uses the [GPL v3 License](./LICENSE).


