# Guide for running benchmarks
To run all of the inputs in benchmarks/inputs through plinegling, lingeling, our DPLL parallel solver and our DPLL sequential solver, run 
```
python3 benchmarks.py `< executables.txt`
```


# Guide for running lingeling and plingeling SAT solvers

For lingeling:

`./benchmarks/lingeling/lingeling input.cnf`

For plingeling:

`./benchmarks/lingeling/plingeling input.cnf <number of threads>`

For instance, to run with 4 threads:

`./benchmarks/lingeling/plingeling input.cnf 4`

Just adjust the path to the lingeling/plingeling solvers as needed!

