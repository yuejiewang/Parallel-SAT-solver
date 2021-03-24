#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>


using namespace std;

string key = "";
vector<vector<string>> originalClauses;

vector<string> splitClause(string s) {
    stringstream ss(s);
    istream_iterator<string> begin(ss);
    istream_iterator<string> end;
    vector<string> vstrings(begin, end);
    return vstrings;
}

void printClauses(vector<vector<string>> clauses) {
    for ( const std::vector<string> &v : clauses )
    {
        for ( string x : v ) std::cout << x << ' ';
        std::cout << std::endl;
    }
}

string DPLL(vector<vector<string>> clauses, map<string, bool> bindings){
    return "Fail";
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
        if(clauseMode == false){
            key += line +"\n";
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
    map<string, bool> m;
    string answer = DPLL(originalClauses, m);
    string output;
    if (answer.compare("Fail") == 0){
        output = "NO SOLUTION";
    } else {
        output = "Placeholder";
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
