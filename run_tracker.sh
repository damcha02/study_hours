#!/bin/bash

git pull

cd build
# recompile if new changes
cmake ..
make 
./tracker