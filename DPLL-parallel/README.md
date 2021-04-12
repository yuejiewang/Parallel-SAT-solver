# DPLL-parallel with load balance

This is a naive implementation of  load balancing.  
Thread number can be defined during compilation.  
Each thread is responsible for a subtree (disjoint) of the binary tree of value assignment in DPLL.  
Program exits when all threads are finished. Earlier finished threads will take some work from other working threads.  
Pseudo code for the algorithm design is in pseudocode.txt (need update).  

## Compile
```
make dpll-parallel [FPOW=pow] [FV=v]
```
`pow` is a non-negative int and thread number will be set to 2^pow  
`v` is either 0 or 1, meaning verbose output for debugging  
**default:** pow = 3 and thread_number = 8, v = 0  

* Clean up
```
make clean  
```
## Run parallel DPLL

Run parallel DPLL with input file `TEST.cnf`  
```
./dpll-parallel TEST.cnf  
```
**OR**  
store `TEST.cnf` under `input/` or `large_input/` and run shell scripts:  
```
./run.sh [-p pow] [-v]
./run_large.sh [-p pow] [-v]  
```
`pow` and `v` are defined the same as in makefile

## Output format

DPLL output is in the form of:  
>if `x` is true, `x` is printed  
>if `y` is false, `-y` is printed  
>if `z` can be either true or false, `\z` is printed  

When running the shell scripts, the output is redirected to `output/TEST.txt` and the correctness is automatically checked by python script: `check_sat.py`, which only checks the correctness if the cnf is satisfiable, and outputs `no solution` if the program doesn't find a solution.  

Run correctness check separately with inputfile `INPUT.cnf` and outputfile `output/OUTPUT.txt`: 
```
python3 check_sat.py -i INPUT.cnf -o OUTPUT.txt
```
## Current test cases and performance: 

Input files with prefix `sat_` are satisfiable and others with prefix `unsat_` are unsatisfiable.
`input\`: tests that return within a few seconds
`large_input\`: tests that could run for several minutes

# TODO

force killing all other threads when one finds a solution  
benchmarks  