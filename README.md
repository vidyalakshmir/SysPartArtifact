# SysPart

This is the repository of SysPart, which is a partitioning tool that automatically identifies partitions within server program based on the system calls, and inserts a system call filter at the partition boundary.

The tool includes two components :
1. An analysis tool which identifies partitions and computes the system calls to be filtered.
2. An enforcement tool that transforms the binary by inserting the system call filter at the partition boundary

Documentation and source code for each component is available at the respective directories in the repository root. See the README in each directory for details on how to build and run each tool.

Cloning

This repository uses several git submodules for dependencies of the tool. To clone this repository, you must use SSH and have SSH access configured for Github.

You can clone the repository recursively with the following command :

git clone --recursive $REPO\_URL 
