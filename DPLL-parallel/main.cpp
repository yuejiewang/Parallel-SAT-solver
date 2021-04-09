/*************************************************************************
	> File Name: main.cpp
	> Author: yuejie
	> Mail: kathywangyuejie@gmail.com 
	> Created Time: Thu Apr  8 11:35:03 2021
 ************************************************************************/

#include<omp.h>
#include"bcp.h"
#include<fstream>
using namespace std;
static const int NTHREADS = 4;
static const int MAXSTRLEN = 1024;
int main(int argc, char* argv[])
{
	list<Node*> local_stack[NTHREADS];
	list<string> sat[NTHREADS];
	ifstream fptr(argv[1]);  // assuming input file is the first command line param
	if (!fptr.is_open()) exit(1);
	CNF_T input_cnf;
	CNF_T root_cnf[NTHREADS];
	int input_num;
	int var_num[NTHREADS];
	memset(var_num, 0, sizeof(var_num));
	while (!fptr.eof()) {
		char buff[1024];
		fptr.getline(buff, 1000);
		stringstream ss_parser;
		ss_parser << buff;
		string substr;
		ss_parser >> substr;
		if (substr == "" || substr == "c") continue;
		else if (substr == "p") {
			ss_parser >> substr;
			int clause_num;
			ss_parser >> input_num >> clause_num;
		}
		else {
			CLAUSE_T clause;
			while (substr != "0") {
				clause.push_back(substr);
				ss_parser >> substr;
			}
			input_cnf.push_back(clause);
		}
	}
	for (int i = 0; i < NTHREADS; i++) {
		var_num[i] = input_num;
		root_cnf[i] = input_cnf;
	}
#pragma omp parallel num_threads(NTHREADS)
	{
#pragma omp for
		for (int i = 0; i < NTHREADS; i++) {
			int tid = omp_get_thread_num();
			BITMAP dec(var_num[tid]), val(var_num[tid]);
			Node* node = new Node(dec, val, root_cnf[tid]);
			node->dec.set(0, 1);
			node->dec.set(1, 1);
			node->val.set(0, tid & 0x1);
			node->val.set(1, (tid >> 1) & 0x1);

			Node* current_node = node;
			while (current_node != NULL) {
				stringstream ss_tid;
				ss_tid << tid;
				string tid_str;
				ss_tid >> tid_str;
				cout << ("----- " + tid_str + " -----\n" + current_node->to_string());
				CNF_T lcnf, rcnf;
				Node* lc = new Node(current_node->dec, current_node->val, lcnf);
				Node* rc = new Node(current_node->dec, current_node->val, rcnf);
				int flag = fake_propagation(current_node, lc, rc);
				switch (flag) {
					case 0:
						current_node = lc;
						local_stack[tid].push_back(rc);
						break;
					case 1:
						break;
					case 2:
						current_node = NULL;
						if (!local_stack[tid].empty()) {
							current_node = local_stack[tid].back();
							local_stack[tid].pop_back();
						}
						break;
					default:
						current_node = NULL;
						break;
				}
				if (flag == 1) break;
			}
			if (current_node != NULL) {
				for (int i = 0; i < current_node->dec.rawlen; i++) {
					stringstream ss;
					ss << (i + 1);
					string str;
					ss >> str;
					if (current_node->dec.get(i) && current_node->val.get(i)) {
						str = "<" + str + " 1> ";
					}
					else if (current_node->dec.get(i) && !current_node->val.get(i)) {
						str = "<" + str + " 0> ";
					}
					else str = "<" + str + " X> ";
					sat[tid].push_back(str);
				}
			}
		}
	}

	for (int i = 0; i < NTHREADS; i++) {
		for (list<string>::iterator ij = sat[i].begin(); ij != sat[i].end(); ij++) {
			cout << *ij;
		}
		cout << endl;
	}
	return(0);
}
