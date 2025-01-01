//
// Created by alimedhat on 28/12/2024.
//


#include <iostream>
#include <stack>
#include <fstream>
#include "Parser.h"

using namespace std;

void Parser::parse(const vector<string>& input, const string &derivation_path) {
    stack<string> parseStack;


    // validate the input contains the end token
    if(input[input.size() - 1] != END){
        cerr << "Error: input should end with $";
        return;
    }
    string temp = "";
    parseStack.push(END);
    parseStack.push(startSymbol);

    size_t inputIndex = 0;
    derivationSteps.clear();
    leftMostDerivation.clear();

    string leftDerivation = startSymbol;
    leftMostDerivation.push_back(leftDerivation);

    while (!parseStack.empty()) {
        if (inputIndex < input.size()) {
            temp += "Stack: " + joinStack(parseStack, " ") + "\n | Input: " + join(input, " ")
                    + "\n | InputIndex: " + input[inputIndex] + " " + to_string(inputIndex)
                    + "\n | Action: ";
        }else{
            temp += "Stack: " + joinStack(parseStack, " ") + "\n | Input: " + join(input, " ")
                    + "\n | Action: ";
        }
        string top = parseStack.top();
        parseStack.pop();
        // case if the stack is empty and there remains inputs
        if (top == END || inputIndex == input.size()) {
            if(inputIndex == input.size() - 1 && top == input[inputIndex]){
                temp += "accept";
            }else{
                // case if the stack is empty and there remains inputs
                // case if the input is empty and stack not empty
                temp += "reject";
            }
            derivationSteps.push_back(temp);
            temp = "";
            break;
        }

        if (terminals.find(top) != terminals.end()) {
            // terminals
            if (inputIndex < input.size() && input[inputIndex] == top) {
                temp += "match " + top;
                inputIndex++;
            } else {
                // case if the terminal in the stack does not match the input token action remove from the stack
                // Missing terminal handling
                leftMostDerivation.push_back("Current derivation (after inserting " + top + "): \n" + leftDerivation);
                temp +="Error: missing "+ top +", inserted to the input" ;
            }
        } else {
            // non-terminals
            vector<string> production = parsingTable.getProduction(top, input[inputIndex]);
            if (production.empty()) {
                // case of error recovery action remove from the input token action discard the input token
                temp += "Error:(illegal "+ top +" ), discard " + input[inputIndex] + ")";
                leftMostDerivation.push_back("Current derivation (after deleting " + input[inputIndex] + "): \n" + leftDerivation);
                parseStack.push(top);
                inputIndex++;
            } else if(production.size() == 1 && production[0] == SYNCH){
                // if production sync then error recovery action remove from stack
                temp += "Error, M["+ top +", "+ input[inputIndex] +"] = synch "+ top +" has been popped";

                // Update the current derivation for the leftmost derivation
                size_t pos = leftDerivation.find(top);
                if (pos != string::npos) {
                    leftDerivation.replace(pos, top.length() + 1, join(production, " ", true));
                    leftMostDerivation.push_back("Current derivation: \n" + leftDerivation);
                }
            } else {
                temp += top + " -> " + join(production, " ");

                // Update the current derivation for the leftmost derivation
                size_t pos = leftDerivation.find(top);
                if (pos != string::npos) {
                    leftDerivation.replace(pos, top.length() + 1, join(production, " ", true));
                    leftMostDerivation.push_back("Current derivation: \n" + leftDerivation);
                }

                for (auto it = production.rbegin(); it != production.rend(); ++it) {
                    if (*it != EPSILON) {
                        parseStack.push(*it);
                    }
                }
            }
        }
        derivationSteps.push_back(temp);
        temp = "";
    }
    string derivation_table_path = derivation_path.substr(0, derivation_path.find_last_of('.')) + "_table.txt";
    string derivation_left_most_path = derivation_path.substr(0, derivation_path.find_last_of('.')) + "_left_most.txt";
    printLeftDerivation(derivation_left_most_path);
    printDerivation(derivation_table_path);
}

void Parser::printDerivation(const string &derivation_path) const {
    ofstream output_file(derivation_path);
    if (!output_file.is_open()) {
        cerr << "Error: Could not open file "
             << derivation_path
             << " for writing." << endl;
        return;
    }
    output_file << "Derivation Steps:" << endl;
    for (const auto& step : derivationSteps) {
        output_file << step << endl;
    }
    cout << "Derivation Steps written to " << derivation_path << endl;
    output_file.close();
}


void Parser::printLeftDerivation() const {
    for (const auto& step : leftMostDerivation) {
        cout << step << endl;
    }
}

void Parser::printDerivation() const {
    for (const auto& step : derivationSteps) {
        cout << step << endl;
    }
}

void Parser::printLeftDerivation(const string &left_most_derivation_path) const {
    ofstream output_file(left_most_derivation_path);
    if (!output_file.is_open()) {
        cerr << "Error: Could not open file "
             << left_most_derivation_path
             << " for writing." << endl;
        return;
    }
    output_file << "Derivation Steps:" << endl;
    for (const auto& step : leftMostDerivation) {
        output_file << step << endl;
    }
    cout << "Derivation Steps written to " << left_most_derivation_path << endl;
    output_file.close();
}

