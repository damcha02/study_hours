#!/bin/bash

mkdir build
cd build

rm CMakeCache.txt

cmake ..
make 

clear 

./setup $1