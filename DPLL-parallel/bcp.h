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
#include<string.h>
#include<cstring>
#include<string>
using namespace std;

struct bitmap {
	int maxlen;
	vector<unsigned char> raw;
	bitmap(int _n) {
		maxlen = (_n + 7) / 8;
		for (int i = 0; i < maxlen; i++)
			raw.push_back(0x00);
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

struct cnf_type {
	vector<vector<int> > expr;
	cnf_type(vector<vector<int> > vec): expr(vec) {}
};
typedef struct cnf_type CNF_TYPE;

typedef string CNF_NAIVE;

struct node_t {
	BITMAP dec;
	BITMAP val;
	CNF_NAIVE cnf;
	node_t(BITMAP _d, BITMAP _v, CNF_NAIVE _c): dec(_d), val(_v), cnf(_c) {}
};
typedef struct node_t Node;

// If we're going to implement more propagation algorithms we might need OOP
// use the following for naive implementation
void propagation(Node* curr, Node* left, Node* right) {
	/* TODO
	 * input: DL(struct decision_level pointer), left_DL(pointer), right_DL(pointer)
	 * output: split_variable
	 * left_DL/right_DL(pointer): the decision level of left/right child
	 * function: compute and assign the value of left_DL and right_DL relatively
	 * do Propagation() on the current node based on the decided variables, update bitmap (decided
	 * and values) and the clause during the while loop, don't need to resolve conflict
	 * choose a split_variable, assign it with 0/1, update only this variable in the bitmap (decided
	 * and values) and give the updated bitmaps and local clause to left/right child pointer
	 * for the local clause, just remove all the variables assigned with a value to simplify the
	 * computation for subtrees
	 */
	return;
}
