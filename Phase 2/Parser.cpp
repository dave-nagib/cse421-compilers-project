//
// Created by alimedhat on 28/12/2024.
//


#include <iostream>
#include <stack>
#include "Parser.h"

using namespace std;

void Parser::parse(const vector<string>& input) {
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

    while (!parseStack.empty()) {
        temp += "Stack: " + joinStack(parseStack, " ") + "\n | Input: " + input[inputIndex] + "\n | Action: ";
        string top = parseStack.top();
        parseStack.pop();
        // case if the stack is empty and there remains inputs
        if (top == END || inputIndex == input.size()) {
            if(inputIndex == input.size() - 1 && top == input[inputIndex]){
//                cout << "TOOOPPP: " <<top << " | "<< " INPPPUUUTT: " << input[inputIndex] << endl;
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
                temp +="Error, expected " + top + " but found " + input[inputIndex];
            }
        } else {
            // non-terminals
            vector<string> production = parsingTable.getProduction(top, input[inputIndex]);
            // if production sync then error recovery action remove from stack this case should be included
            if (production.empty()) {
                // case of error recovery action remove from the input token action discard the input token
                temp += "Error:(illegal "+ top +" ), discard " + input[inputIndex] + ")";
                parseStack.push(top);
                inputIndex++;
            } else if(production.size() == 1 && production[0] == SYNCH){
                temp += "Error, M["+ top +", "+ input[inputIndex] +"] = synch "+ top +" has been popped";
            } else {
                temp += top + " -> " + join(production, " ");
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
    printDerivation();
}

void Parser::printDerivation() const {
    for (const auto& step : derivationSteps) {
        cout << step << endl;
    }
}

