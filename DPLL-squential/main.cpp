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
#include "inputReader.h"
#include "dpll.h"
namespace fs = std::filesystem;

using namespace std;


vector<vector<string>> originalClauses;

void runWithInputFile(string inputFileName) {
    // Create a text string, which is used to output the text file
    string line;
    string problem_type;
    int num_variables;
    int num_clauses;
    // Read from the text file
    ifstream inputFile(inputFileName);
    string filename = inputFileName.substr(9);
    filename.erase(filename.find_last_of("."), string::npos);


    // Use a while loop together with the getline() function to read the file line by line
    cout << "reading: " << inputFileName << endl;
    while (getline(inputFile, line)) {
        // Output the text from the file
        if (line[0] == 'p') {
            problem_type = line[2];
            num_variables = (int) line[4];
            num_clauses = (int) line[6];
        } else if (line[0] != 'c' && (isnumber(line[0]) || line[0] == '-')) {
            vector<string> clauseStrings = splitIntoClauses(line);
            for (int i = 0; i < clauseStrings.size(); i++)
                originalClauses.push_back(splitClause(clauseStrings[i]));
        }
    }
//    cout << "Original Clauses: " << endl;
//    printClauses(originalClauses);

    // Close the file
    inputFile.close();

    // Run the actual DPLL algorithm
    vector<string> atoms = makeAtomList(originalClauses);
    map<string, bool> b;
    State original = State{originalClauses, b};
    string answer = DPLL(original, atoms);
    string output;
    if (answer.compare("Fail") == 0) {
        output = "NO SOLUTION";
    } else {
        output = answer;
    }

    // Create and open output file
    ofstream outputFile("../output/sequential_"+filename+"_output.txt");
    cout << "writing to ../output/sequential_"+filename+"_output.txt" << endl;

    // Write to the file
    outputFile << output + "\n";

    // Close the file
    outputFile.close();
}

void runWithAllInputFiles(){
    string path = "../input";
    for (const auto & entry : fs::directory_iterator(path)) {
        std::cout << "running on input file: " << entry.path() << std::endl;
        runWithInputFile(entry.path());
    }
}


int main(int argc, char *argv[]) {
    if(argc == 2){
        runWithInputFile(argv[1]);
    } else {
        runWithAllInputFiles();
    };
    return 0;
}
