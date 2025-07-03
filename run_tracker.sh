#!/bin/bash

cd build
# recompile if new changes
rm CMakeCache.txt

cmake ..
make 

clear
./tracker