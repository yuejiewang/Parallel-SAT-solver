#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>
#include <cctype>
#include <algorithm>
#include <iterator>
using namespace std;

#ifndef DPLL_SQUENTIAL_INPUTREADER_H
#define DPLL_SQUENTIAL_INPUTREADER_H

static inline void ltrim(std::string &s);
static inline void rtrim(std::string &s);
static inline void trim(std::string &s);
vector<string> splitIntoClauses(string s);
vector<string> splitClause(string s);
void printClauses(vector<vector<string>> clauses);
vector<string> makeAtomList(vector<vector<string>> clauses);

#endif //DPLL_SQUENTIAL_INPUTREADER_H
