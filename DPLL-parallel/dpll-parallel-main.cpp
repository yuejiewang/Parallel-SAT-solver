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
#include"../utils/dpll.h"
#include"../utils/inputReader.h"
using namespace std;
static const int NTHREADS = 16;
static const int POW = 4;
typedef vector<string> CLAUSE;
typedef vector<CLAUSE> CNF;
typedef map<string, bool> BIND;

static omp_lock_t lock[NTHREADS];
list<State*> local_stack[NTHREADS];
vector<State> sat;
int global_flag = 0;
omp_lock_t iolock;

#define set(x) omp_set_lock(&lock[x])
#define unset(x) omp_unset_lock(&lock[x])

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

// get tid for a busy thread using random strategy
inline int getBusyThread() {
	int randi = rand() % NTHREADS;
	if (!local_stack[randi].empty()) return randi;
	randi = rand() % NTHREADS;
	if (!local_stack[randi].empty()) return randi;
	randi = rand() % NTHREADS;
	if (!local_stack[randi].empty()) return randi;
	// after generating three random numbers, just pick the first busy thread...
	
	for (int t = 0; t < NTHREADS; t++) {
		if (!local_stack[t].empty()) {
			return t;
		}
	}
	
	return -1;
}

// thread ctid finished, thread rtid busy, sharing work from rtid to ctid
// critical section
inline State* loadShare(int btid) {
	if (btid == -1) return NULL;
	// sequentially set lock to avoid deadlock
	set(btid);
	State* t = NULL;
	if (!local_stack[btid].empty()) {
		t = local_stack[btid].front();
		local_stack[btid].pop_front();
	}
	unset(btid);
	return t;
}

// need to clear the stack for a thread that has found an answer
inline void clearStack(int ctid) {
	set(ctid);
	local_stack[ctid].clear();
//	for (int i = 0; i < local_stack[ctid].size(); i++)
//		local_stack[ctid].pop_back();
//		delete *ii;
	unset(ctid);
}

int main(int argc, char* argv[])
{
	omp_init_lock(&iolock);
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
			for (size_t i = 0; i < clauseStrings.size(); i++)
				originalClauses.push_back(splitClause(clauseStrings[i]));
		}
	}
	inputFile.close();

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
		for (int pid = 0; pid <	NTHREADS; pid++)
		{
			omp_init_lock(&lock[pid]);
		}

#pragma omp barrier
		do {
			int end_flag = 0;
			int tid = omp_get_thread_num();
			stringstream ss_tid;
			ss_tid << tid;
			string tid_str;
			ss_tid >> tid_str;  // for debug
			if (local_stack[tid].empty() || global_flag) break;
// ---------------------------- critical section ----------------------------------------
//			set(tid);
			State* root = local_stack[tid].back();
			local_stack[tid].pop_back();
//			unset(tid);
// ---------------------------- end critical section ------------------------------------
			if (root == NULL) break;
			if (root->clauses.empty()) {
				end_flag = 1;
				clearStack(tid);
				global_flag = 1;
				omp_set_lock(&iolock);
				sat.push_back(*root);
				omp_unset_lock(&iolock);
				break;
			} else if (containsEmptyClause(root->clauses)) {
				root = NULL;
				end_flag = 2;
				clearStack(tid);
				break;
			}
			
			*root = handleEasyCases(*root);
			if (root->clauses.empty()) {
				end_flag = 1;
				clearStack(tid);
				global_flag = 1;
				omp_set_lock(&iolock);
				sat.push_back(*root);
				omp_unset_lock(&iolock);
				break;
			}
			if (containsEmptyClause(root->clauses)) {
				root = NULL;
				end_flag = 2;
				clearStack(tid);
				break;
			}
			// compute the root node for each thread if nthreads > 1
			// NTHREADS = 2^POW
			for (int p = 0; p < POW; p++) {
				string split = nextUnboundAtom(*root);
				bool value = (bool)((tid >> p) & 0x1);
				if (split == "") break;
				*root = propagate(*root, split, value);
				if (root->conflict) {
					root = NULL;
					end_flag = 2;
					break;
				}
				if (root->clauses.empty()) {
					end_flag = 1;
					clearStack(tid);
					global_flag = 1;
					omp_set_lock(&iolock);
					sat.push_back(*root);
					omp_unset_lock(&iolock);
					break;
				}
				*root = handleEasyCases(*root);
				if (root->conflict) {
					root = NULL;
					end_flag = 2;
					break;
				}
				if (root->clauses.empty()) {
					end_flag = 1;
					clearStack(tid);
					global_flag = 1;
					omp_set_lock(&iolock);
					sat.push_back(*root);
					omp_unset_lock(&iolock);
					break;
				}
			}

//  #pragma omp barrier
			if (end_flag == 1) break;
			if (end_flag == 2) {
				root = NULL;
				int busy = getBusyThread();
				if (busy != -1) root = loadShare(busy);
			}

			State* current_state = root;  // subtree traversal for this thread
			while (current_state != NULL) {
				if (global_flag) break;
				if (current_state->conflict) {
					if (local_stack[tid].empty()) {
						delete current_state;
						current_state = NULL;
						end_flag = 2;
						if (global_flag) break;
						int busy = getBusyThread();
						if (busy != -1) current_state = loadShare(busy);
						continue;
					}  // case UNSAT for this thread
					else {
						delete current_state;
						current_state = NULL;
						if (global_flag) break;
// ---------------------------- critical section ----------------------------------------
						set(tid);
						if (local_stack[tid].empty()) current_state = NULL;
						else {
							current_state = local_stack[tid].back();
							local_stack[tid].pop_back();
						}
						unset(tid);
// ---------------------------- end critical section ------------------------------------
						continue;
					}  // current branch UNSAT, pop local stack if not empty
				}
				if (current_state->clauses.empty()) {
					end_flag = 1;
					clearStack(tid);
					global_flag = 1;
					omp_set_lock(&iolock);
					sat.push_back(*current_state);
					omp_unset_lock(&iolock);
					break;
				}  // case SAT for this thread

				*current_state = handleEasyCases(*current_state);
				if (current_state->conflict) {
					if (local_stack[tid].empty()) {
						delete current_state;
						current_state = NULL;
						end_flag = 2;
						if (global_flag) break;
						int busy = getBusyThread();
						if (busy != -1) current_state = loadShare(busy);
						continue;
					}  // case UNSAT for this thread
					else {
						delete current_state;
						current_state = NULL;
						if (global_flag) break;
// ---------------------------- critical section ----------------------------------------

						set(tid);
						if (local_stack[tid].empty()) current_state = NULL;
						else {
							current_state = local_stack[tid].back();
							local_stack[tid].pop_back();
						}
						unset(tid);
// ---------------------------- end critical section ------------------------------------
						continue;
					}
				}  // current branch UNSAT, pop local stack if not empty
				if (current_state->clauses.empty()) {
					end_flag = 1;
					clearStack(tid);
					global_flag = 1;
					omp_set_lock(&iolock);
					sat.push_back(*current_state);
					omp_unset_lock(&iolock);
					break;
				}  // case SAT for this thread
				// split current node, go to left child and push right child
				State copy = *current_state;
				string split = nextUnboundAtom(copy);
				if (split == "") break;
				State* lstate = new State;
				*lstate = propagate(copy, split, false);
				State* rstate = new State;
				*rstate = propagate(copy, split, true);
				delete current_state;
				current_state = lstate;
				if (global_flag) break;
// ---------------------------- critical section ----------------------------------------
				set(tid);
				local_stack[tid].push_back(rstate);
				unset(tid);
// ---------------------------- end critical section ------------------------------------
			}
		} while(0);

#pragma omp barrier
#pragma omp for
		for (int pid = 0; pid <	NTHREADS; pid++)
		{
			int tid = omp_get_thread_num();
			omp_destroy_lock(&lock[tid]);
		}
	}

	if (!sat.empty()) {
		string outstr = toString(sat[0].bindings, atoms);
		cout << outstr;
	}
	
	omp_destroy_lock(&iolock);
	if (!global_flag) cout << "No solution\n";
	return(0);
}
