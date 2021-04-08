/*************************************************************************
	> File Name: main.cpp
	> Author: yuejie
	> Mail: kathywangyuejie@gmail.com 
	> Created Time: Thu Apr  8 11:35:03 2021
 ************************************************************************/

#include<iostream>
#include<omp.h>
#include<sstream>
#include<list>
#include"bcp.h"
using namespace std;
static const int NTHREADS = 4;
int main()
{
	list<Node*> local_stack[NTHREADS];
#pragma omp parallel num_threads(NTHREADS)
	{
#pragma omp for
		for (int i = 0; i < 10; i++) {
			int tid = omp_get_thread_num();
			stringstream ss;
			string str_tid, str_i;
			ss << tid << " " << i;
			ss >> str_tid >> str_i;
			string str = str_i + " of thread " + str_tid + "\n";
			BITMAP_NAIVE dec, val;
			CNF_NAIVE cnf = str;
			Node* node = new Node(dec, val, cnf);
			local_stack[tid].push_back(node);
		}
#pragma omp for
		for (int i = 0; i < NTHREADS; i++) {
			while (!local_stack[i].empty()) {
				cout << local_stack[i].front()->cnf;
				local_stack[i].pop_front();
			}
		}
	}
	return(0);
}
