#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <algorithm>
#include <iterator>
#include "inputReader.h"
#include "dpll.h"

using namespace std;


// trim from start (in place)
static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}

vector<string> splitIntoClauses(string s) {
    trim(s);
    vector<string> clauseList;
    size_t pos0 = 0;
//    cout << "split into clauses: " + s << endl;
    while ((pos0 = s.find(" 0")) != std::string::npos) {
        clauseList.push_back(s.substr(0, pos0));
        s.erase(0, pos0 + 2);
    }
    if(s.size() != 0){
        clauseList.push_back(s);
    }
    return clauseList;
}

vector<string> splitClause(string s) {
    trim(s);
    stringstream ss(s);
    istream_iterator<string> begin(ss);
    istream_iterator<string> end;
    vector<string> vstrings(begin, end);
    return vstrings;
}

void printClauses(vector<vector<string>> clauses) {
    for (const vector<string> &v : clauses) {
        for (string x : v) cout << x << ' ';
        cout << endl;
    }
}

vector<string> makeAtomList(vector<vector<string>> clauses) {
    vector<string> atoms;
    for (int i = 0; i < clauses.size(); i++) {
        vector<string> clause = clauses[i];
        for (int j = 0; j < clause.size(); j++) {
            string literal = clause[j];
            string atom = atom_of(literal);
            auto atomIndex = find(atoms.begin(), atoms.end(), atom);
            if (atomIndex == atoms.end()) {
                atoms.push_back(atom);
            }
        }
    }
    return atoms;
}