# DPLL-parallel

This is a naive implementation of load balancing and thread communication.  
Thread number can be defined during compilation.  
Each thread is responsible for a subtree (disjoint) of the binary tree of value assignment in DPLL.  
Program exits when all threads are finished. Earlier finished threads will take some work from other working threads.  
Pseudo code for the algorithm design see below.  

## Compile

> make dpll-parallel [FPOW=pow] [FV=v]

`pow` is a non-negative int and thread number will be set to 2^pow  
`v` is either 0 or 1, meaning verbose output for debugging  
***default:*** pow = 3 and thread_number = 8, v = 0  

* Clean up

> make clean  

## Run parallel DPLL

Run parallel DPLL with input file `TEST.cnf`  

> ./dpll-parallel-p${pow} TEST.cnf  

**OR**  
store `TEST.cnf` under `input/` or `large_input/` and run shell scripts:  
> ./run.sh [-p pow] [-v]
> ./run_large.sh [-p pow] [-v]  

`pow` and `v` are defined the same as in makefile
For cims crunchy machines, first run `module load gcc-9.2` before running the programming.  

## Output format

DPLL output is in the form of:  
> if `x` is true, `x` is printed  
> if `y` is false, `-y` is printed  
> if `z` can be either true or false, `\z` is printed

When running the shell scripts, the output is redirected to `TEST.txt` in the corresponding output directory and the correctness is automatically checked by python script: `check_sat.py`, which only checks the correctness if the cnf is satisfiable, and outputs `no solution` if the program doesn't find a solution.  

Run correctness check separately with inputfile `INPUT.cnf` and outputfile `OUTPUT.txt`: 
> python3 check_sat.py -i INPUT.cnf -o OUTPUT.txt  

## Current test cases: 

Input files with prefix `sat_` are satisfiable and others with prefix `unsat_` are unsatisfiable.  
`input\`: tests that return within a few seconds  
`large_input\`: tests that could run for several minutes  

## Benchmarks
> ./run_benchmark.sh
> ./run_benchmark_large.sh

Will automatically run the tests in input/ and input_large/ with 1 2 4 8 16 32 64 threads and save the execution time in `benchmark_small_p${POW}_b{BATCH}.txt` and `benchmark_large_p${POW}_b{BATCH}.txt`
The tests will run multiple times to take the average exectution time.


# Pseudo Code

This is the pseudo code for parallel DPLL  

* data structure for nodes:  
``` c++
struct State {
	bool conflict;  // if this->clauses contains an empty clause
    vector<vector<string>> clauses;  // the cnf expression
    map<string, bool> bindings;  // the set of assigned values
};
```

* the master thread before fork()
``` c++
// push the global root node (State) into the local stack of each thread  
for (tid = 0; tid < NTHREADS; tid++)  
	local_stack[tid].push_back(global_root);  
```

* for each individual thread:  
``` c++
1.	local_stack[tid].pop_back() and calculate the root of the subtree  
2.	root<-propagation(root);  // reference to handleEasyCases() from utils/dpll.cpp  
	current_state<-root;  
3.	while (current_state != NULL)  // current node: either coming from searching through its own subtree or from work sharing with other threads  
4.		if (current_state is solved) // case: current state finds a solution  
			return result;  
5.		else if (current_state.conflict)  // case: current assigned values cannot satisfy the CNF  
6.			if (local_stack.empty())  // case: current subtree finished with no solution  
7.				if (find_busy_thread())  // case: there is still thread not finished  
					share_work();  
					continue;  
8.				else  // case: no other thread running  
					return "no solution"  
9.			else  // case: continue searching through the subtree  
				current_state = local_stack.pop_back();  
				continue;  
10.		current_state<-propagation(current_state)  // case: current state is not a terminating state  
11.		repeat step 4~9  // check for termination  
		// case: current node is not a leaf  
12.		find_next_decide_atom();  // reference to propagate() from utils/dpll.cpp  
13.		current_state<-left_child;  
14.		local_stack.push_back(right_child);  
```

* propagation  

***`propagate()`***
assign the chosen atom with a certain value and update the related clauses.
  
***`handleEasyClauses()`***
according to the current state, assign values for clauses that contains only one atom and "pure literals".

# TODO

* look for more efficient ways to kill all threads when a solution is found  
* change to `pthreads` for precise control over thread behaviors  