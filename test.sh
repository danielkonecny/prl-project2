#!/bin/bash

mat1=$(head -n1 mat1)
mat2=$(head -n1 mat2)

cpus=$((mat1*mat2))

mpic++ -o mm mm.cpp mm.h -std=c++0x
mpirun --use-hwthread-cpus --oversubscribe -np $cpus mm
rm -f mm
