#!/usr/bin/python3.4

import subprocess

path = "./bin/Release/cache-effect-benchmarks"
result_path = "results/"

bench_list = []
with open("benchmark_list.txt", "r") as file:
    bench_list = [line.strip().rstrip() for line in file]

for bench in bench_list:
    print("Starting benchmark: {}".format(bench))
#    print([path, bench])
    with open(result_path + bench + ".txt", "w") as result_file:
        subprocess.call([path, bench], stdout=result_file)
    print("Finished benchmark: {}".format(bench))
