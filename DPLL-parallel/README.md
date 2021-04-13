This is a naive version of parallel DPLL

Thread number is statically defined and stored in NTHREADS in dpll-parallel-main.cpp

Each thread is responsible for a subtree (disjoint) of the binary tree of value assignment in DPLL

Program exits when all threads are finished. No load-balancing is implemented, so the execution time is the maximum of all threads.

Pseudo code for the algorithm design is in pseudocode.txt (need update).

Compilation:
make dpll-parallel

Clean up:
make clean

Run parallel DPLL with inputfile named TEST.cnf:
./dpll-parallel TEST.cnf

OR store TEST.cnf in input/ or large_input/ directory and run the shell scripts:
Run small tests: ./run.sh
Run large tests: ./run_large.sh

The DPLL output is in the form of:
if a variable x is true, "${x}" is printed
if a variable y is false, "-${y}" is printed
if a variable z can be either true or false, "/${z}" is printed

When running the shell scripts, the output is redirected to output/TEST.txt and the correctness is automatically checked by python script: check_sat.py, which only checks the correctness if the cnf is satisfiable, and outputs "no solution" if the program doesn't find a solution.

Run correctness check separately with inputfile INPUT.cnf and outputfile OUTPUT.txt:
python3 check_sat.py -i INPUT.cnf -o OUTPUT.txt

Current test cases and performance:
input: test case that terminate within a few seconds
large_input: could run for several minutes

TODO:
work sharing (stealing) among threads
force killing all other threads when one finds a solution
benchmarks
correctness check when unsatisfiable (maybe just look for existing unsatisfiable test cases)

