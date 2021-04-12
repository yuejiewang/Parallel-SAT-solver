FPOW=3
FV=0
CFLAGS = -fopenmp -std=c++17 -O -DPOW=${FPOW} -DVFLAG=${FV}
dpll-parallel: ../utils/dpll.o ../utils/inputReader.o dpll-parallel-main.cpp
	g++ ${CFLAGS} -o dpll-parallel dpll-parallel-main.cpp ../utils/dpll.o ../utils/inputReader.o

clean:
	rm -f dpll-parallel
