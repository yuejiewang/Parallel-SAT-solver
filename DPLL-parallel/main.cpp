/*************************************************************************
	> File Name: main.cpp
	> Author: yuejie
	> Mail: kathywangyuejie@gmail.com 
	> Created Time: Thu Apr  8 11:35:03 2021
 ************************************************************************/

#include<omp.h>
#include"bcp.h"
using namespace std;
static const int NTHREADS = 4;
int main()
{
	list<Node*> local_stack[NTHREADS];
#pragma omp parallel num_threads(NTHREADS)
	{
#pragma omp for
		for (int i = 0; i < NTHREADS; i++) {
			int tid = omp_get_thread_num();
			BITMAP dec(8), val(8);
			char str[128] = "1 2 3 4 5 6 7 8";
			char* ptr = NULL;
			char* substr = strtok_r(str, " ", &ptr);
			CLAUSE_T clause;
			while (substr != NULL) {
				string tmp(substr);
				clause.push_back(tmp);
				substr = strtok_r(NULL, " ", &ptr);
			}
			CNF_T cnf;
			cnf.push_back(clause);
			Node* node = new Node(dec, val, cnf);
			node->dec.set(0, 1);
			node->dec.set(1, 1);
			node->val.set(0, tid & 0x1);
			node->val.set(1, (tid >> 1) & 0x1);

			local_stack[tid].push_back(node);
			CNF_T lcnf, rcnf;
			Node* lc = new Node(node->dec, node->val, lcnf);
			Node* rc = new Node(node->dec, node->val, rcnf);
			if (fake_propagation(node, lc, rc) == 0) {
				local_stack[tid].push_back(lc);
				local_stack[tid].push_back(rc);
			}
		}
	}

	for (int i = 0; i < NTHREADS; i++) {
		cout << "stack " << i << endl;
		while (!local_stack[i].empty()) {
			Node* curr = local_stack[i].front();
			for (int j = 0; j < 8; j++) cout << curr->dec.get(j);
			cout << "/";
			for (int j = 0; j < 8; j++) cout << curr->val.get(j);
			cout << endl;
			for (CNF_T::iterator ii = curr->cnf.begin(); ii != curr->cnf.end(); ii++) {
				for (CLAUSE_T::iterator ij = (*ii).begin(); ij != (*ii).end(); ij++) {
					cout << *(ij) << " ";
				}
				cout << endl;
			}
			local_stack[i].pop_front();
		}
	}
	return(0);
}
