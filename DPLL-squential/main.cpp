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


vector<vector<string>> originalClauses;
vector<string> atoms;

struct State {
    vector<vector<string>> clauses;
    map<string, bool> bindings;
};

vector<string> splitIntoClauses(string s) {
    trim(s);
    vector<string> clauseList;
    size_t pos0 = 0;
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


string atom_of(string literal) {
    if (literal[0] == '-') {
        literal.erase(0, 1);
    }
    return literal;
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

string stringifyBindings(map<string, bool> bindings) {
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

string DPLL(State s) {
    vector<vector<string>> clauses = s.clauses;
    map<string, bool> bindings = s.bindings;
    if (clauses.size() == 0) { // done! solution found
        return stringifyBindings(bindings);
    }
    if (containsEmptyClause(clauses)) {
        return "Fail";
    }
    State easyCasesResult = handleEasyCases(s);
    clauses = easyCasesResult.clauses;
    bindings = easyCasesResult.bindings;
    if (clauses.size() == 0) { // done! solution found
        return stringifyBindings(bindings);
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
    string answer = DPLL(stateCopyTrue);
    if (answer != "Fail") { // found a solution! just backfill the ones that can be either with true
        return answer;
    }
    // see if we find a solution with the unbound atom bound to false
    State stateCopyFalse = State{clauses, bindings};
    stateCopyFalse = propagate(stateCopyFalse, unboundAtom, false);
    return DPLL(stateCopyFalse);
}

void runWithInputFile(string inputFileName) {
    // Create a text string, which is used to output the text file
    string line;
    string problem_type;
    int num_variables;
    int num_clauses;
    // Read from the text file
    ifstream inputFile(inputFileName);

    // Use a while loop together with the getline() function to read the file line by line
    cout << "reading: " << inputFileName << endl;
    while (getline(inputFile, line)) {
        // Output the text from the file
        if (line[0] == 'p') {
            problem_type = line[2];
            num_variables = (int) line[4];
            num_clauses = (int) line[6];
        } else if (line[0] != 'c' && isnumber(line[0])) {
            vector<string> clauseStrings = splitIntoClauses(line);
            for (int i = 0; i < clauseStrings.size(); i++)
                originalClauses.push_back(splitClause(clauseStrings[i]));
        }
    }
    cout << "Original Clauses: " << endl;
    printClauses(originalClauses);

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
    }

    // Create and open output file
    ofstream outputFile("../sequential_dpll_output.txt");

    // Write to the file
    outputFile << output + "\n";
    cout << "writing to: sequential_dpll_output.txt" << endl;

    // Close the file
    outputFile.close();
}


int main() {
    runWithInputFile("../uf50-0999.cnf");
    return 0;
}
