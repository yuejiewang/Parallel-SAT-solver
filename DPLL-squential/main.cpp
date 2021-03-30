#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>
#include <cctype>
#include <algorithm>
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


string key = "";
vector<vector<string>> originalClauses;
vector<string> atoms;

struct State {
    vector<vector<string>> clauses;
    map<string, bool> bindings;
};


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


string atom_of(string literal) {
    string s;
    if (literal[0] == '-') {
        s.push_back(literal[1]);
    } else {
        s.push_back(literal[0]);
    }
    return s;
}


bool value_of(string literal) {
    return literal[0] != '-';
}


void makeAtomList(vector<vector<string>> clauses) {
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
}

bool containsEmptyClause( vector<vector<string>> clauses){
    for(int i = 0; i < clauses.size(); i++){
        if(clauses[i].size() == 0){
            return true;
        }
    }
    return false;
}

State propagate(State s, string atom, bool value) {
    s.bindings.insert(pair<string, bool>(atom, value));
    string literal, opposite;
    if (value == true) {
        literal = atom;
        opposite = "-" + atom;
    } else {
        literal = "-" + atom;
        opposite = atom;
    }
    vector<vector<string>> result_clauses;
    for (int i = 0; i < s.clauses.size(); i++) {
        vector<string> clause = s.clauses[i];
        vector<string>::iterator oppIndex = find(clause.begin(), clause.end(), opposite);
        if (oppIndex != clause.end()) { // if opposite in clause
            clause.erase(oppIndex);
        }
        vector<string>::iterator litIndex = find(clause.begin(), clause.end(), literal);
        if (litIndex == clause.end()) { // if literal is not in clause
            result_clauses.push_back(clause);
        }
    }
    return State{result_clauses, s.bindings};
}

State handleEasyCases(State s) {
    vector<vector<string>> clauses = s.clauses;
    map<string, bool> bindings = s.bindings;

    bool stillChanging = true;
    map<string, bool> atomAppearances;
    map<string, string> pureLiterals;
    State result = s;
    while (stillChanging) {
        map<string, bool> oldBindings(bindings);
        // look for case 1: singleton clause
        for (int i = 0; i < clauses.size(); i++) {
            vector<string> clause = clauses[i];
            if (clause.size() == 1) {
                result = propagate(result, atom_of(clause[0]), value_of(clause[0]));
            } else {
                // look for case 2: pure literal
                for (int j = 0; j < clause.size(); j++) {
                    string literal = clause[j];
                    string atom = atom_of(literal);
                    map<string, bool>::iterator iter = atomAppearances.find(atom);
                    if (iter != atomAppearances.end()) { // if atom in atomAppearances
                        bool atomValue = iter->second;
                        map<string, string>::iterator iterLit = pureLiterals.find(atom);
                        if (atomValue != value_of(literal) && iterLit != pureLiterals.end()) {
                            pureLiterals.erase(atom);
                        }
                    } else {
                        atomAppearances.insert(pair<string, bool>(atom, value_of(literal)));
                        pureLiterals.insert(pair<string, string>(atom, literal));
                    }
                }
            }
        }
        for (auto const&[key, literal] : pureLiterals) {
            result = propagate(result, atom_of(literal), value_of(literal));
        }
        if (oldBindings == s.bindings) {
            stillChanging = false;
        }
    }
    return result;
}

string nextUnboundAtom(State s){
    vector<string> unbound;
    for(int i = 0; i < s.clauses.size(); i++){
        for(int j = 0; j < s.clauses[i].size(); j++){
            string atom = atom_of(s.clauses[i][j]);
            if(s.bindings.find(atom) == s.bindings.end()){
                unbound.push_back(atom);
            }
        }
    }
    sort(unbound.begin(), unbound.end());
    return unbound[0];
}

string stringifyBindings(map<string, bool> bindings) {
    map<int, bool> intBindings;
    for (auto const&[key, val] : bindings) {
        intBindings.insert(pair<int, bool>(stoi(key), val));
    }
    string result;
    for (auto const&[key, val] : bindings) {
        result += key + " ";
        result += val + "\n";
    }
    return result;
}

string DPLL(State s) {
    vector<vector<string>> clauses = s.clauses;
    map<string, bool> bindings = s.bindings;
    if(clauses.size() == 0){
        return stringifyBindings(bindings);
    }
    if(containsEmptyClause(clauses)){
        return "Fail";
    }
    State easyCasesResult = handleEasyCases(s);
    if(clauses.size() == 0){
        return stringifyBindings(bindings);
    }
    if(containsEmptyClause(clauses)){
        return "Fail";
    }
    vector<vector<string>> clausesCopy(clauses);
    map<string, bool> bindingsCopy(bindings);
    State stateCopy = State{clausesCopy, bindingsCopy};
    string unboundAtom = nextUnboundAtom(stateCopy);
    stateCopy = propagate(stateCopy, unboundAtom, true);
    string answer = DPLL(stateCopy);
    if(answer == "Fail"){
        for(int i = 0; i < atoms.size(); i++){
            string atom = atoms[i];
            for(int j = 0; j < answer.size(); j++){
                if(answer[j] == atom[0]){
                    break;
                }
            }
            answer += atom + " true\n";
        }
        return answer;
    }
    stateCopy = propagate(stateCopy, unboundAtom, false);
    return DPLL(stateCopy);


    //line 92

    return DPLL(s);
}

void runWithInputFile(string inputFileName) {
    // Create a text string, which is used to output the text file
    string line;

    // Read from the text file
    ifstream inputFile(inputFileName);

    // Use a while loop together with the getline() function to read the file line by line
    cout << "reading: " << inputFileName << endl;
    bool clauseMode = true;
    while (getline(inputFile, line)) {
        // Output the text from the file
        if (clauseMode == false) {
            key += line + "\n";
        } else if (line.compare("0") != 0 && clauseMode) {
            originalClauses.push_back(splitClause(line));
        } else {
            clauseMode = false;
            continue;
        }

    }
    cout << "Original Clauses: " << endl;
    printClauses(originalClauses);
    cout << "Key:\n" + key << endl;

    // Close the file
    inputFile.close();

    // Run the actual DPLL algorithm
    makeAtomList(originalClauses);
    map<string, bool> b;
    State original = State{originalClauses, b};
    string answer = DPLL(original);
    string output;
    if (answer.compare("Fail") == 0) {
        output = "NO SOLUTION";
    } else {
        output = answer;
        // '\n'.join([f"{k} {str(v)[0]}" for k, v in sorted(answer.items(), key=lambda i: int(i[0]))]))
    }

    // Create and open output file
    ofstream outputFile("../sequential_dpll_output.txt");

    // Write to the file
    outputFile << output + "\n" + key;
    cout << "writing to: sequential_dpll_output.txt" << endl;

    // Close the file
    outputFile.close();
}


int main() {
    runWithInputFile("../dpll_input.txt");
    return 0;
}
