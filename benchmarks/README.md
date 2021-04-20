# Guide for running benchmarks
To run all of the inputs in benchmarks/inputs through plinegling, lingeling, our DPLL parallel solver and our DPLL sequential solver, run: 
```
python3 benchmarks.py `< executables.txt`
```
### Notes
- For the parallel solvers, we will run with thread count going from 1-64 in increments of powers of 2
- `executables.txt` is simply a file which contains the set of command line arguments that `benchmarks.py` expects. You can edit it or pass in command line arguments manually
    - the format for adding a sequential solver is an `s` followed by the path to the executable of the solver
    - the format for adding a parallel solver is a `p` followed by the path to the executable
    - for any parallel solvers other than our own `dpll-parallel` it is expected that the solver takes arguments in the same form as `plingeling` described below
    - sequential solvers just take the input filename as an argument
- the default timeout is 200 seconds. To edit this, change the last line in `executables.txt`
- Each run of `benchmarks.py` will run each solver and thread count combination on each input in `benchmarks/inputs` and record the time it took in seconds
  - it will create a new csv of the timing results. This allows you to run the script multiple times and take averages of the results for analysis.

# Guide for running lingeling and plingeling SAT solvers

To clone from github and install the lingeling/plingeling solvers do

```
chmod +x get_lingeling_solvers.sh
./get_lingeling_solvers.sh
```

For lingeling:

`./benchmarks/lingeling/lingeling input.cnf`

For plingeling:

`./benchmarks/lingeling/plingeling input.cnf <number of threads>`

For instance, to run with 4 threads:

`./benchmarks/lingeling/plingeling input.cnf 4`

Just adjust the path to the lingeling/plingeling solvers as needed!

