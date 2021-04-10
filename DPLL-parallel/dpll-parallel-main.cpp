/*************************************************************************
	> File Name: dpll-parallel-main.cpp
	> Author: yuejie
	> Mail: kathywangyuejie@gmail.com
	> Created Time: Thu Apr  8 11:35:03 2021
 ************************************************************************/

#include<iostream>
#include<stdlib.h>
#include<stdio.h>
#include<map>
#include<vector>
#include<list>
#include<string.h>
#include<cstring>
#include<string>
#include<sstream>
#include<omp.h>
#include<fstream>
#include"dpll.h"
#include"inputReader.h"
using namespace std;
static const int NTHREADS = 64;
static const int POW = 6;
typedef vector<string> CLAUSE;
typedef vector<CLAUSE> CNF;
typedef map<string, bool> BIND;
string toString(BIND b, vector<string> a) {
	map<int, string> bmap;
	string str = "";
	for (size_t j = 0; j < a.size(); j++) {
		string atom = a[j];
		if (b.find(atom) != b.end()) {
			if (b[atom] == false) bmap[stoi(atom)] = "-" + atom;
			else bmap[stoi(atom)] = atom;
		} else {
			bmap[stoi(atom)] = "/" + atom;
		}
	}
	for (auto const&[key, val] : bmap) {
		str += val;
		str += "\n";
	}
	str += "\n";
	return str;
}
int main(int argc, char* argv[])
{
	CNF originalClauses;
	string line;
	string problem_type;
	int num_variables;
	int num_clauses;
	ifstream inputFile(argv[1]);
	string filename(argv[1]);
	filename.erase(filename.find_last_of("."), string::npos);

	while (getline(inputFile, line)) {
		line.erase(0, line.find_first_not_of(" \n\t"));
		if (line[0] == 'p') {
			problem_type = line[2];
			num_variables = (int)line[4];
			num_clauses = (int)line[6];
		} else if (line[0] == 'c') {
			continue;
		} else if (isdigit(line[0]) || line[0] == '-') {
			CLAUSE clauseStrings = splitIntoClauses(line);
			for (int i = 0; i < clauseStrings.size(); i++)
				originalClauses.push_back(splitClause(clauseStrings[i]));
		}
	}
	inputFile.close();

	list<State*> local_stack[NTHREADS];
	vector<State> sat[NTHREADS];

	vector<string> atoms = makeAtomList(originalClauses);
	BIND b;
	State original = State{ false, originalClauses, b };
	// Push the root State into each local stack
	for (int i = 0; i < NTHREADS; i++) {
		State* rootCopy = new State;
		*rootCopy = original;
		local_stack[i].push_back(rootCopy);
	}

#pragma omp parallel num_threads(NTHREADS)
	{
#pragma omp for
		for (int pid = 0; pid < NTHREADS; pid++) {
			int end_flag = 0;
			int tid = omp_get_thread_num();
			stringstream ss_tid;
			ss_tid << tid;
			string tid_str;
			ss_tid >> tid_str;  // for debug
			if (local_stack[tid].empty()) exit(0);
			State* root = local_stack[tid].back();
			local_stack[tid].pop_back();
			if (root->clauses.empty()) {
				end_flag = 1;
				sat[tid].push_back(*root);
			} else if (containsEmptyClause(root->clauses)) end_flag = 2;
			do {
				if (end_flag) break;
				
				*root = handleEasyCases(*root);
				if (root->clauses.empty()) {
					end_flag = 1;
					sat[tid].push_back(*root);
					break;
				}
				if (containsEmptyClause(root->clauses)) {
					end_flag = 2;
					break;
				}

				// compute the root node for each thread if nthreads > 1
				// NTHREADS = 2^POW
				for (int p = 0; p < POW; p++) {
					string split = nextUnboundAtom(*root);
					bool value = (bool)((tid >> p) & 0x1);
					*root = propagate(*root, split, value);
					if (root->conflict) {
						end_flag = 2;
						break;
					}
					if (root->clauses.empty()) {
						end_flag = 1;
						sat[tid].push_back(*root);
						break;
					}
					*root = handleEasyCases(*root);
					if (root->conflict) {
						end_flag = 2;
						break;
					}
					if (root->clauses.empty()) {
						end_flag = 1;
						sat[tid].push_back(*root);
						break;
					}
				}
				if (end_flag) break;

				State* current_state = root;  // subtree traversal for this thread
				while (current_state) {
					if (current_state->conflict) {
						if (local_stack[tid].empty()) {
							end_flag = 2;
							break;
						}  // case UNSAT for this thread
						delete current_state;
						current_state = local_stack[tid].back();
						local_stack[tid].pop_back();
						continue;
					}  // current branch UNSAT, pop local stack if not empty
					if (current_state->clauses.empty()) {
						end_flag = 1;
						sat[tid].push_back(*current_state);
						break;
					}  // case SAT for this thread

					*current_state = handleEasyCases(*current_state);
					if (current_state->conflict) {
						if (local_stack[tid].empty()) {
							end_flag = 2;
							break;
						}  // case UNSAT for this thread
						delete current_state;
						current_state = local_stack[tid].back();
						local_stack[tid].pop_back();
						continue;
					}  // current branch UNSAT, pop local stack if not empty
					if (current_state->clauses.empty()) {
						end_flag = 1;
						sat[tid].push_back(*current_state);
						break;
					}  // case SAT for this thread
					// split current node, go to left child and push right child
					State copy = *current_state;
					string split = nextUnboundAtom(copy);
					State* lstate = new State;
					*lstate = propagate(copy, split, false);
					State* rstate = new State;
					*rstate = propagate(copy, split, true);
					delete current_state;
					current_state = lstate;
					local_stack[tid].push_back(rstate);
				}
			}while(0);
		}
	}

	int satFlag = 0;
	for (int i = 0; i < NTHREADS; i++) {
		if (!sat[i].empty()) {
			string outstr = toString(sat[i][0].bindings, atoms);
			cout << outstr;
			satFlag = 1;
			break;
		}
	}
	if (!satFlag) cout << "No solution\n";
	return(0);
}
