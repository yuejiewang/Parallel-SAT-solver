dpll-parallel: dpll.h dpll.cpp inputReader.h inputReader.cpp dpll-parallel-main.cpp
	g++ -g -fopenmp -Wall -o dpll-parallel inputReader.cpp dpll.cpp dpll-parallel-main.cpp

dpll-naive-parallel: bcp.h naive-main.cpp
	g++ -g -fopenmp -Wall -o dpll-naive-parallel naive-main.cpp

clean:
	rm -f dpll-parallel dpll-naive-parallel
