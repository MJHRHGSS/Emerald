#!/bin/bash
cd "$(dirname "$(readlink -f "$0")")"
cd ..
gcc src/emerald.c -o out.out.o 
out/out.o tests/test.emr >> logs/emr_$(date +"%Y-%m-%d_%H-%M-%S").log 2>&1
