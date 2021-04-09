#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>
#include <cctype>
#include <algorithm>
#include <iterator>
#include "dpll.h"

using namespace std;

string atom_of(string literal) {
    if (literal[0] == '-') {
        literal.erase(0, 1);
    }
    return literal;
}


bool value_of(string literal) {
    return literal[0] != '-';
}

bool containsEmptyClause(vector<vector<string>> clauses) {
    for (int i = 0; i < clauses.size(); i++) {
        if (clauses[i].size() == 0) {
            return true;
        }
    }
    return false;
}

State propagate(State s, string atom, bool value) {
    s.bindings.insert(pair<string, bool>(atom, value));
    string literal, opposite;
    if (value) {
        literal = atom;
        opposite = "-" + atom;
    } else {
        literal = "-" + atom;
        opposite = atom;
    }
    vector<vector<string>> result_clauses;
    for (int i = 0; i < s.clauses.size(); i++) {
        vector<string> clause = s.clauses[i];
        auto oppIndex = find(clause.begin(), clause.end(), opposite);
        if (oppIndex != clause.end()) { // if opposite in clause
            clause.erase(oppIndex);
        }
        auto litIndex = find(clause.begin(), clause.end(), literal);
        if (litIndex == clause.end()) { // if literal is not in clause
            result_clauses.push_back(clause);
        }
    }
    return State{result_clauses, s.bindings};
}

State handleEasyCases(State s) {
    bool stillChanging = true;
    while (stillChanging) {
        map<string, bool> oldBindings(s.bindings);
//        cout << "starting with " << s.clauses.size() << " clauses: " << endl;
//        printClauses(s.clauses);
        State resultState = State{s.clauses, s.bindings};
        for (int i = 0; i < s.clauses.size(); i++) {
            vector<string> clause = s.clauses[i];
            if (clause.size() == 1) { // case 1: singleton clause
                string literal = clause[0];
                resultState = propagate(resultState, atom_of(literal), value_of(literal));
            }
        }
        map<string, bool> atomAppearances;
        map<string, string> pureLiterals;
        for (int i = 0; i < resultState.clauses.size(); i++) {
            vector<string> clause = resultState.clauses[i];
            for (int j = 0; j < clause.size(); j++) {
                string literal = clause[j];
                string atom = atom_of(literal);
                map<string, bool>::iterator iter = atomAppearances.find(atom);
                if (iter != atomAppearances.end()) { // if atom in atomAppearances
                    bool atomValue = iter->second;
                    map<string, string>::iterator iterLit = pureLiterals.find(atom);
                    if (atomValue != value_of(literal) && iterLit != pureLiterals.end()) { // not a pure literal
                        pureLiterals.erase(atom);
                    }
                } else {
                    atomAppearances.insert(pair<string, bool>(atom, value_of(literal)));
                    pureLiterals.insert(pair<string, string>(atom, literal));
                }
            }
        }
        for (auto const&[atom, literal] : pureLiterals) { // handle case 2: pure literals
            resultState = propagate(resultState, atom, value_of(literal));
        }
        if (oldBindings == resultState.bindings) {
            stillChanging = false;
        }
        s = resultState;
    }
    return s;
}

string nextUnboundAtom(State s) {
    vector<string> unbound;
    for (int i = 0; i < s.clauses.size(); i++) {
        for (int j = 0; j < s.clauses[i].size(); j++) {
            string atom = atom_of(s.clauses[i][j]);
            if (s.bindings.find(atom) == s.bindings.end()) { // atom not in bindings
                unbound.push_back(atom);
            }
        }
    }
    sort(unbound.begin(), unbound.end());
    return unbound[0];
}

string stringifyBindings(map<string, bool> bindings, vector<string> atoms) {
    map<int, bool> intBindings;
    vector<string> either;
    for (int i = 0; i < atoms.size(); i++) {
        string atom = atoms[i];
        if (bindings.find(atom) == bindings.end()) { // atom not in bindings
            // add to list "either"
            either.push_back(atom);
            // add to bindings with value true
            bindings.insert(pair<string, bool>(atom, true));
        }
    }
    for (auto const&[key, val] : bindings) {
        intBindings.insert(pair<int, bool>(stoi(key), val));
    }
    string result = "";
    for (auto const&[key, val] : intBindings) {
        result += to_string(key) + " ";
        if (find(either.begin(), either.end(), to_string(key)) != either.end()) {
            result += "true or false";
        } else {
            result += val ? "true" : "false";
        };
        result += "\n";
    }
    return result;
}


string DPLL(State s, vector<string> atoms) {
    vector<vector<string>> clauses = s.clauses;
    map<string, bool> bindings = s.bindings;
    if (clauses.size() == 0) { // done! solution found
        return stringifyBindings(bindings, atoms);
    }
    if (containsEmptyClause(clauses)) {
        return "Fail";
    }
    State easyCasesResult = handleEasyCases(s);
    clauses = easyCasesResult.clauses;
    bindings = easyCasesResult.bindings;
    if (clauses.size() == 0) { // done! solution found
        return stringifyBindings(bindings, atoms);
    }
    if (containsEmptyClause(clauses)) {
        return "Fail";
    }
//    vector<vector<string>> clausesCopy(clauses);
//    map<string, bool> bindingsCopy(bindings);
    State stateCopyTrue = State{clauses, bindings};
    string unboundAtom = nextUnboundAtom(stateCopyTrue);
    // see if we find a solution with the unbound atom bound to true
    stateCopyTrue = propagate(stateCopyTrue, unboundAtom, true);
    string answer = DPLL(stateCopyTrue, atoms);
    if (answer != "Fail") { // found a solution! just backfill the ones that can be either with true
        return answer;
    }
    // see if we find a solution with the unbound atom bound to false
    State stateCopyFalse = State{clauses, bindings};
    stateCopyFalse = propagate(stateCopyFalse, unboundAtom, false);
    return DPLL(stateCopyFalse, atoms);
}