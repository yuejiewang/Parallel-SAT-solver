import csv
import os
import sys
import time
import subprocess

"""
See README.md for details on arguments to pass in and behavior
"""

TIMEOUT = 60

sequential_executables = []
parallel_executables = []

i = 1
while i < len(sys.argv):
    if sys.argv[i] == "t":
        TIMEOUT = float(sys.argv[i + 1])
    if sys.argv[i] == "s":
        sequential_executables.append(sys.argv[i + 1])
    if sys.argv[i] == "p":
        parallel_executables.append(sys.argv[i + 1])
    i += 2

print(sequential_executables)
print(parallel_executables)

fields = ["Model", "Benchmark", "Thread Count", "Time (s)"]
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
        if pow is not None:
            thread_count = 2 ** pow
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
    for pow in range(7):  # 0-6
        get_benchmark_results(par, pow=pow)

output_filename = "benchmark_results"
i = 0
while os.path.exists(f"{output_filename}{i}.csv"):
    i += 1

with open(f"{output_filename}{i}.csv", 'w') as f:
    write = csv.writer(f)
    write.writerow(fields)
    write.writerows(rows)
    print(f"done writing output to '{output_filename}{i}.csv'")
