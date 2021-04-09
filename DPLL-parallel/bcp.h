/*************************************************************************
	> File Name: bcp.h
	> Author: yuejie
	> Mail: kathywangyuejie@gmail.com 
	> Created Time: Thu Apr  8 09:58:33 2021
 ************************************************************************/
#include<iostream>
#include<stdlib.h>
#include<stdio.h>
#include<vector>
#include<list>
#include<string.h>
#include<cstring>
#include<string>
#include<sstream>
using namespace std;

struct bitmap {
	int maxlen;
	int rawlen;
	unsigned char* raw;
	bitmap(int _n) {
		rawlen = _n;
		maxlen = (_n + 7) / 8;
		raw = new unsigned char[maxlen];
		memset(raw, 0, maxlen * sizeof(unsigned char));
	}
	bitmap() {
		maxlen = 0;
		rawlen = 0;
		raw = NULL;
	}
	void copy(bitmap _b) {
		if (rawlen != _b.rawlen) {
			rawlen = _b.rawlen;
			maxlen = _b.maxlen;
			raw = new unsigned char[maxlen];
		}
		for (int i = 0; i < maxlen; i++)
			raw[i] = _b.raw[i];
	}
	int get_zero_index() {
		for (int i = 0; i < maxlen; i++) {
			if (raw[i] != 0xff) {
				unsigned char c = raw[i];
				for (int j = 0; j < 8; j++) {
					if ((c & 0x01) != 0x01) {
						int idx = i * 8 + j;
						if (idx < rawlen) return idx;
						return -1;
					}
					c = c >> 1;
				}
			}
		}
		return -1;
	}
	int get(int idx) {
		if (idx >= maxlen * 8 || idx < 0) return 0;
		size_t hi = idx >> 3, lo = idx & 0x07;  // hi = idx / 8, lo = idx % 8
		unsigned char d = raw[hi];
		d = d >> lo;
		d &= 0x01;
		return (int)d;
	}
	void set(int idx, int val) {
		if (idx >= maxlen * 8 || idx < 0) return;
		val &= 0x01;
		size_t hi = idx >> 3, lo = idx & 0x07;  // hi = idx / 8, lo = idx % 8
		unsigned char i = 0x01 << lo, v = raw[hi];
		val = val << lo;
		v &= (0xff ^ i);
		v |= val;
		raw[hi] = v;
	}
};
typedef struct bitmap BITMAP;

typedef vector<int> BITMAP_NAIVE;

// maybe can use std::list here, faster on deletion
typedef vector<string> CLAUSE_T;
typedef vector<CLAUSE_T> CNF_T;  // when changing clause type change these two lines only

typedef string CNF_NAIVE;

struct node_t {
	BITMAP dec;
	BITMAP val;
	CNF_T cnf;
	node_t(BITMAP _d, BITMAP _v, CNF_T _c): cnf(_c) {
		dec.copy(_d);
		val.copy(_v);
	}
	int remove_true_clauses_naive() {  // a naive implementation for deleting clauses whose value is already decided
		if (cnf.empty()) exit(1);  // shouldn't be empty
		for (CNF_T::iterator ii = cnf.begin(); ii != cnf.end();) {
			if ((*ii).empty()) exit(1);  // shouldn't be empty
			bool rm_flag = false;
			for (CLAUSE_T::iterator ij = (*ii).begin(); ij != (*ii).end();) {
				string var_str = *ij;
				stringstream ss;
				ss << var_str;
				int var_idx;
				ss >> var_idx;
				if (var_idx < 0) {  // assuming clause variables start from index 1 (no zero)
					var_idx = (~var_idx);  // var_idx = -var_idx
					if (dec.get(var_idx)) {
						if (val.get(var_idx)) (*ii).erase(ij);
						else {
							rm_flag = true;
							break;
						}
					}
					else ij++;
				}
				else {
					var_idx--;
					if (dec.get(var_idx)) {
						if (!val.get(var_idx)) (*ii).erase(ij);
						else {
							rm_flag = true;
							break;
						}
					}
					else ij++;
				}
			}
			if (rm_flag) cnf.erase(ii);
			else if ((*ii).empty()) return 2;  // branch no-sat
			else ii++;
		}
		if (cnf.empty()) return 1;  // branch sat
		return 0;  // branch not finished
	}
	string to_string() {
		string str = "";
		stringstream ss1;
		for (int i = 0; i < dec.rawlen; i++) ss1 << dec.get(i);
		string tmp1 = "";
		ss1 >> tmp1;
		str = tmp1 + "/";
		stringstream ss2;
		for (int i = 0; i < val.rawlen; i++) ss2 << val.get(i);
		string tmp2 = "";
		ss2 >> tmp2;
		str = str + tmp2 + "\n";
		for (CNF_T::iterator ii = cnf.begin(); ii != cnf.end(); ii++) {
			for (CLAUSE_T::iterator ij = (*ii).begin(); ij != (*ii).end(); ij++) {
				str = str + *ij + " ";
			}
			str += "\n";
		}
		return str;
	}
};
typedef struct node_t Node;

int fake_propagation(Node* curr, Node* left, Node* right) {  // no propagation, for test only
	if (curr == NULL) return -1;
	int flag = curr->remove_true_clauses_naive();
	if (flag == 0) {  // banch not finished
		int idx = curr->dec.get_zero_index();
		left->dec.set(idx, 1);
		right->dec.set(idx, 1);
		left->val.set(idx, 0);
		right->val.set(idx, 1);
		left->cnf = curr->cnf;
		right->cnf = curr->cnf;
	}
	return flag;
}

// If we're going to implement more propagation algorithms we might need OOP
// use the following for naive implementation
void propagation(Node* curr, Node* left, Node* right) {
	/* TODO
	 * input: DL(struct decision_level pointer), left_DL(pointer), right_DL(pointer)
	 * output: split_variable
	 * left_DL/right_DL(pointer): the decision level of left/right child
	 * function: compute and assign the value of left_DL and right_DL relatively
	 * do Propagation() on the current node based on the decided variables, update bitmap (decided
	 * and values) and the clause during the while loop
	 * choose a split_variable, assign it with 0/1, update only this variable in the bitmap (decided
	 * and values) and give the updated bitmaps and local clause to left/right child pointer
	 * for the local clause, just remove all the variables assigned with a value to simplify the
	 * computation for subtrees
	 */
	return;
}
