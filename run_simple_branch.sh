#!/bin/sh

clang++ -std=c++11 simple_branch_if.cpp -o simple_branch_if
clang++ -std=c++11 simple_branch_cond.cpp -o simple_branch_cond

./simple_branch_if > results/branch_bench_if.txt
./simple_branch_cond > results/branch_bench_cond.txt

rm simple_branch_if simple_branch_cond
