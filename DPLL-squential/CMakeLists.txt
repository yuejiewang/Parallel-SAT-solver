cmake_minimum_required(VERSION 3.17)
project(DPLL_squential)

set(CMAKE_CXX_STANDARD 17)

add_executable(DPLL_squential main.cpp dpll.h dpll.cpp inputReader.cpp inputReader.h)