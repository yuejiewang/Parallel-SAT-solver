# SAT Solver Parallelization Project (Group 14)

#### Lorenzo Meninato (lm4244), Nellie Spektor (ns104), Yuejie Wang (yw4989)

## Run Instructions

### Parallel Solver

###### Navigate to the DPLL-parallel directory

> cd DPLL-parallel

###### Compile

> make dpll-parallel [FV=v]

`v` is either 0 or 1, meaning verbose output for debugging  
***default:*** v = 0

For cims crunchy machines, first run `module load gcc-9.2` before running the programming.

###### Run

input file `INPUT.cnf`, with 2^pow threads, and with an optional benchmark file `BENCHMARK.csv` to output the execution
time

> ./dpll-parallel -p ${pow} INPUT.cnf [BENCHMARK.csv]

**OR**  
to run with all input in `input/` or `large_input/`, you can run the following shell scripts:
> ./run.sh [-p pow] [-v]  
> ./run_large.sh [-p pow] [-v]

When running the shell scripts, the output is redirected to `output_parallel_p${pow}/TEST.txt` and the correctness is
automatically checked by python script: `check_sat.py`, which only checks the correctness if the cnf is satisfiable, and
outputs `no solution` if the program doesn't find a solution.

`pow` and `v` are defined the same as before

Pseudo code for our Parallel Solver and additional information are in `DPLL-parallel/README.md`

### Sequential Solver

###### Navigate to the DPLL-squential directory

> cd DPLL-squential

###### Compile

> make dpll-sequential [FV=v]

###### Run

> ./dpll-sequential INPUT.cnf

this generates an output file in `/sequential_output`

### Output format

DPLL output (sequential and parallel solvers) is in the form of:
> if `x` is true, `x` is printed  
> if `y` is false, `-y` is printed  
> if `z` can be either true or false, `\z` is printed

OR if the solver determines that the input is unsatisfiable:
> NO SOLUTION

### Tests

Run a single correctness check separately with inputfile `INPUT.cnf` and outputfile `OUTPUT.txt`:
> python3 tests/check_sat.py -i INPUT.cnf -o OUTPUT.txt

This checks that the output file (in the format above) is a satisfactory solution to the input file which contains the
initial clauses in Conjunctive Normal Form (CNF). If the input is unsatisfiable, the output file is expected to
contain "NO SOLUTION"

We have a python test suite which can verify that all of the output generated by the parallel and sequential solvers is
correct, given all of the input in the `input` directory. Run all of the tests like this:
> python3 tests/test_correctness_of_outputs.py

Note: This does not run the solvers, it just checks the output that has already been generated

### Benchmarks

To run all of the inputs in benchmarks/inputs through plinegling, lingeling, our DPLL parallel solver and our DPLL
sequential solver, run:

> python3 benchmarks/benchmarks.py `< executables.txt`

Additional information on how the benchmark script works and what it does in `benchmarks/README.md`

## File Structure

- `/benchmarks`
    - scripts for downloading plingeling and lingeling modern solvers (for comparison)
    - `/input` sample inputs which include satisfiable and unsatisfiable cnf examples of varying sizes
    - `benchmarks.py` which runs each solver on all inputs in `/input`
    - plots which show the results of our benchmarks and are showcased in our paper
    - benchmark result files
- `/DPLL-parallel`
    - `dpll-parallel-main.cpp` contains the code for the parallel solver
    - `README.md` contains the pseudo code and description of the parallel solver
    - run scripts for running the solver on various inputs and taking timing measurements
- `/DPLL-squential`
    - `dpll.cpp` contains the code for the sequential dpll algorithm and helper functions
    - `inputReader.cpp` contains helper functions which deal with reading the input
    - `main.cpp` runs the sequential algorithm on all files in local input directory or in a specific file if given
- `/tests`
    - `check_sat.py` takes an input and an output file and checks that the output is correct
    - `test_correctness_of_outputs.py` looks through input and output directories and checks that the outputs are all
      correct
        - assumes the following directories exist and have files in them: /input, /sequential_output,
          /output_parallel_p0, /output_parallel_p1, /output_parallel_p2 /output_parallel_p3, and /output_parallel_p4
- `/utils`
    - `dpll.cpp` and `inputReader.cpp`: versions of the `dpll` and `inputReader` programs in `/DPLL-squential` which are
      specialized for the parallel solver
- various input and output directories