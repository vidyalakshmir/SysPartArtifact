#!/bin/bash

compiler=$1
app=$2
cd compiler_optimizations/$compiler
BASEDIR=$(pwd)
scriptname=make_$app.sh
./$scriptname

