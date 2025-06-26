#!/bin/bash

cd build
# recompile if new changes
cmake ..
make 
./tracker