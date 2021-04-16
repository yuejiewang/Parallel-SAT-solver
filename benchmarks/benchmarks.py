"""plingeling
lingeling
DPLL sequential
DPLL parallel

for each of those, varying the number of threads (1-64), time all the test cases and take an average

benchmark.py expected behavior:
 1. take list of executables as commandline input
 2. run each executable on each input in benchmarks/inputs
    - time each run and keep it in a file called benchmark_results.csv which has columns:
        Model,Benchmark,Thread Count,Time (ms)


- benchmarks script (lorenzo to send plingeeling and lingeling instructions and sample cnf files to test with)"""

import csv
import os
import sys
import time
import subprocess

TIMEOUT = 1

sequential_executables = []
parallel_executables = []

i = 1
while i < len(sys.argv):
    if sys.argv[i] == "s":
        sequential_executables.append(sys.argv[i + 1])
    if sys.argv[i] == "p":
        parallel_executables.append(sys.argv[i + 1])
    i += 2

print(sequential_executables)
print(parallel_executables)

fields = ["Model", "Benchmark", "Thread Count", "Time (ms)"]
rows = []

input_dir = "inputs"
input_files = []
for (dirpath, dirnames, filenames) in os.walk(input_dir):
    input_files.extend([os.path.join(dirpath, file) for file in filenames])

print("input files: ", input_files)


def get_benchmark_results(executable, pow=None):
    thread_count = 1
    for input_file in input_files:
        run_time = 0
        run_command = ""
        if pow:
            thread_count = 2**pow
            if "dpll-parallel" in executable:
                run_command += executable + " -p " + str(pow) + " " + input_file
            else:
                run_command += executable + " " + input_file + " " + str(thread_count)
        else:
            run_command += executable + " " + input_file
        run_command += " > output.txt"
        print("timing: ", run_command)
        start = time.time()
        try:
            subprocess.call(run_command, shell=True, timeout=TIMEOUT, stdout=subprocess.DEVNULL)
            run_time = time.time() - start
        except subprocess.TimeoutExpired:
            run_time = -1
        # run_time = timeit(stmt=run_command, setup="import subprocess")
        print(run_time)
        model = os.path.basename(executable)
        row = [model, input_file[7:], thread_count, run_time]
        rows.append(row)


for seq in sequential_executables:
    get_benchmark_results(seq)

for par in parallel_executables:
    for pow in range(6):
        get_benchmark_results(par, pow=pow)

with open('benchmark_results.csv', 'w') as f:
    # using csv.writer method from CSV package
    write = csv.writer(f)

    write.writerow(fields)
    write.writerows(rows)
