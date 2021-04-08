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
			BITMAP dec(8), val(8);
			CNF_NAIVE cnf = str;
			Node* node = new Node(dec, val, cnf);
			node->dec.set(0, 1);
			node->dec.set(1, 0);
			node->dec.set(2, 1);
			node->dec.set(3, 0);
			node->dec.set(4, 1);
			node->dec.set(5, 0);
			node->dec.set(6, 1);
			node->dec.set(7, 0);

			node->val.set(0, 0);
			node->val.set(1, 1);
			node->val.set(2, 0);
			node->val.set(3, 1);
			node->val.set(4, 0);
			node->val.set(5, 1);
			node->val.set(6, 0);
			node->val.set(7, 1);
			local_stack[tid].push_back(node);
		}
	}

	for (int i = 0; i < NTHREADS; i++) {
		while (!local_stack[i].empty()) {
			Node* curr = local_stack[i].front();
			for (int j = 0; j < 8; j++) cout << curr->dec.get(j);
			cout << "/";
			for (int j = 0; j < 8; j++) cout << curr->val.get(j);
			cout << endl;
			cout << curr->cnf;
			local_stack[i].pop_front();
		}
	}
	return(0);
}
