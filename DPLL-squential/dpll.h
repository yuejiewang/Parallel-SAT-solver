#include <string>
#include <map>
#include <vector>

#ifndef DPLL_SQUENTIAL_DPLL_H
#define DPLL_SQUENTIAL_DPLL_H
using namespace std;

struct State {
    bool conflict;
    vector<vector<string>> clauses;
    map<string, bool> bindings;
};

string atom_of(string literal);
bool value_of(string literal);
bool containsEmptyClause(vector<vector<string>> clauses);
State propagate(State s, string atom, bool value);
State handleEasyCases(State s);
string nextUnboundAtom(State s);
string stringifyBindings(map<string, bool> bindings, vector<string> atoms);
string DPLL(vector<string> atoms, vector<State> stack);


#endif //DPLL_SQUENTIAL_DPLL_H
