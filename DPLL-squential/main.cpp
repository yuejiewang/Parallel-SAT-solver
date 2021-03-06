#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>
#include <cctype>
#include <algorithm>
#include <iterator>
#include <filesystem>

#include"dpll.h"
#include"inputReader.h"

namespace fs = std::filesystem;

using namespace std;

typedef vector<string> CLAUSE;
typedef vector<CLAUSE> CNF;
typedef map<string, bool> BIND;


void runWithInputFile(string inputFileName) {
    vector <vector<string>> originalClauses;
    // Create a text string, which is used to output the text file
    string line;
    // Read from the text file
    ifstream inputFile(inputFileName);
    string filename =  fs::path( inputFileName ).stem();


    // Use a while loop together with the getline() function to read the file line by line
    cout << "reading : " << inputFileName << endl;
    while (getline(inputFile, line)) {
        line.erase(0, line.find_first_not_of(" \n\t"));
        // Output the text from the file
        if (line[0] == 'p') {
            continue;
        } else if (line[0] != 'c' && (isdigit(line[0]) || line[0] == '-')) {
            vector <string> clauseStrings = splitIntoClauses(line);
            for (int i = 0; i < clauseStrings.size(); i++)
                originalClauses.push_back(splitClause(clauseStrings[i]));
        }
    }
//    cout << "Original Clauses: " << endl;
//    printClauses(originalClauses);

    // Close the file
    inputFile.close();

    // Run the actual DPLL algorithm
    vector <string> atoms = makeAtomList(originalClauses);
    map<string, bool> b;
    State original = State{false, originalClauses, b};
    vector<State> stack;
    stack.push_back(original);
    string answer = DPLL(atoms, stack);
    string output;
    if (answer.compare("Fail") == 0) {
        output = "NO SOLUTION";
    } else {
        output = answer;
    }

    // Create and open output file
    ofstream outputFile("../sequential_output/" + filename + ".txt");
    cout << "writing to ../sequential_output/" + filename + ".txt" << endl;

    // Write to the file
    outputFile << output + "\n";

    // Close the file
    outputFile.close();
}

void runWithAllInputFiles() {
    string path = "../input";
    for (const auto &entry : fs::directory_iterator(path)) {
        std::cout << "running on input file: " << entry.path() << std::endl;
        runWithInputFile(entry.path());
    }
}


int main(int argc, char *argv[]) {
    if (argc == 2) {
        runWithInputFile(argv[1]);
    } else {
        runWithAllInputFiles();
    }
    return 0;
}
