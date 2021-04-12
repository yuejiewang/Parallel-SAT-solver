FPOW=3
CFLAGS = -fopenmp -std=c++17 -O -DPOW=${FPOW}
dpll-parallel: ../utils/dpll.o ../utils/inputReader.o dpll-parallel-main.cpp
	g++ ${CFLAGS} -o dpll-parallel dpll-parallel-main.cpp ../utils/dpll.o ../utils/inputReader.o

clean:
	rm -f dpll-parallel
