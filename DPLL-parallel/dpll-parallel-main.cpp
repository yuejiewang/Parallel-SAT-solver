/*************************************************************************
	> File Name: dpll-parallel-main.cpp
	> Author: yuejie
	> Mail: kathywangyuejie@gmail.com
	> Created Time: Thu Apr  8 11:35:03 2021
 ************************************************************************/

#include<iostream>
#include<iomanip>
#include<stdlib.h>
#include<stdio.h>
#include<map>
#include<vector>
#include<list>
#include<string.h>
#include<cstring>
#include<string>
#include<sstream>
#include<fstream>
#include<cmath>
#include<unistd.h>
#include<sys/time.h>
#include<omp.h>

#include"../utils/dpll.h"
#include"../utils/inputReader.h"

using namespace std;

//static const int NTHREADS = 16;
//static const int POW = 4;
//static const bool VFLAG = false;
typedef vector<string> CLAUSE;
typedef vector<CLAUSE> CNF;
typedef map<string, bool> BIND;

static const int NTHREADS = pow(2, POW);
static omp_lock_t lock[NTHREADS];
list<State*> local_stack[NTHREADS];
vector<State> sat;
int global_flag = 0;
omp_lock_t iolock;
struct timeval timeS, timeE;

#define set(x) omp_set_lock(&lock[x])
#define unset(x) omp_unset_lock(&lock[x])

string toString(BIND b, vector<string> a) {
	map<int, string> bmap;
	string str = "";
	for (size_t j = 0; j < a.size(); j++) {
		string atom = a[j];
		if (b.find(atom) != b.end()) {
			if (b[atom] == false) bmap[stoi(atom)] = "-" + atom;
			else bmap[stoi(atom)] = " " + atom;
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
//	usleep(1);
	int randi = rand() % NTHREADS;
//	if (!local_stack[randi].empty()) return randi;
//	randi = rand() % NTHREADS;
//	if (!local_stack[randi].empty()) return randi;
//	randi = rand() % NTHREADS;
//	if (!local_stack[randi].empty()) return randi;
	// after generating three random numbers, just pick the first busy thread...
	for (int t = randi; t < NTHREADS + randi; t++) {
		int tmpi = t % NTHREADS;
		if (!local_stack[tmpi].empty()) {
			return tmpi;
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
	if (!local_stack[btid].empty() && !global_flag) {
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
	cout << "# NTHREADS=" << NTHREADS << endl;
	gettimeofday(&timeS, NULL);
	omp_init_lock(&iolock);
	CNF originalClauses;
	string line;
	string problem_type;
	int num_variables;
	int num_clauses;
	if (argc < 2) {
		cout << "no input" << endl;
		exit(1);
	}
	ifstream inputFile(argv[1]);
	ofstream benchmarkFile;
	string filename(argv[1]);
	if (!inputFile.is_open()) {
		cout << "cannot open input file" << endl;
		exit(1);
	}
	filename.erase(0, filename.find_last_of("/") + 1);
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
		int tid = omp_get_thread_num();
		omp_init_lock(&lock[tid]);

#pragma omp barrier
		do {
			int end_flag = 0;
			stringstream ss_tid;
			ss_tid << tid;
			string tid_str;
			ss_tid >> tid_str;  // for debug
			State* root = NULL;
// ---------------------------- critical section ----------------------------------------
			set(tid);
			if (local_stack[tid].empty()) {
				end_flag = 2;
			} else {
				if (!global_flag) {
					root = local_stack[tid].back();
					local_stack[tid].pop_back();
				}
			}
			unset(tid);
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
				if (busy != -1) {
					root = loadShare(busy);
					if (VFLAG) {
						omp_set_lock(&iolock);
						cout << "# thread " << tid << " takes from thread " << busy << endl;
						omp_unset_lock(&iolock);
					}
				}
			}

			State* current_state = root;  // subtree traversal for this thread
			while (current_state != NULL) {
//				if (global_flag) break;
				if (current_state->conflict) {
					if (local_stack[tid].empty()) {
						delete current_state;
						current_state = NULL;
						end_flag = 2;
//						if (global_flag) break;
						int busy = getBusyThread();
						if (busy != -1) {
							current_state = loadShare(busy);
							if (VFLAG) {
								omp_set_lock(&iolock);
								cout << "# thread " << tid << " takes from thread " << busy << endl;
								omp_unset_lock(&iolock);
							}
						}
						continue;
					}  // case UNSAT for this thread
					else {
						delete current_state;
						current_state = NULL;
//						if (global_flag) break;
// ---------------------------- critical section ----------------------------------------
						set(tid);
						if (local_stack[tid].empty()) current_state = NULL;
						else {
							if (!global_flag) {
								current_state = local_stack[tid].back();
								local_stack[tid].pop_back();
							}
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
//						if (global_flag) break;
						int busy = getBusyThread();
						if (busy != -1) {
							current_state = loadShare(busy);
							if (VFLAG) {
								omp_set_lock(&iolock);
								cout << "# thread " << tid << " takes from thread " << busy << endl;
								omp_unset_lock(&iolock);
							}
						}
						continue;
					}  // case UNSAT for this thread
					else {
						delete current_state;
						current_state = NULL;
//						if (global_flag) break;
// ---------------------------- critical section ----------------------------------------

						set(tid);
						if (local_stack[tid].empty()) current_state = NULL;
						else {
							if (!global_flag) {
								current_state = local_stack[tid].back();
								local_stack[tid].pop_back();
							}
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
//				if (global_flag) break;
// ---------------------------- critical section ----------------------------------------
				set(tid);
				local_stack[tid].push_back(rstate);
				unset(tid);
// ---------------------------- end critical section ------------------------------------
			}
		} while(0);

		if (VFLAG) {
			omp_set_lock(&iolock);
			cout << "thread " << tid << " finished" << endl;
			omp_unset_lock(&iolock);
		}

#pragma omp barrier
		omp_destroy_lock(&lock[tid]);
	}

	omp_destroy_lock(&iolock);

	if (!sat.empty()) {
		string outstr = toString(sat[0].bindings, atoms);
		cout << outstr;
	}
	if (sat.empty()) cout << "No solution\n";

	gettimeofday(&timeE, NULL);
	long int interval = (timeE.tv_sec - timeS.tv_sec) * 1000000 + (timeE.tv_usec - timeS.tv_usec);
	double exec_time_sec = interval * 1e-6;
	double exec_time_ms = interval * 0.001;
	if (argc >= 3) {
		benchmarkFile.open(argv[2], ios::app | ios::out);
	}
	if (benchmarkFile.is_open()) {
		benchmarkFile.setf(std::ios::left);
		benchmarkFile << setw(25) << filename << " ";
		benchmarkFile << std::fixed;
		benchmarkFile << setprecision(6) << setw(10) << exec_time_sec << " ";
		benchmarkFile << setprecision(3) << setw(10) << exec_time_ms << endl;
	} else {
		printf("# execution-time(ms): %.6f %.3f\n", exec_time_sec, exec_time_ms);
	}
	return(0);
}
