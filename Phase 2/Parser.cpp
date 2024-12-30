//
// Created by alimedhat on 28/12/2024.
//


#include <iostream>
#include <stack>
#include "Parser.h"

using namespace std;

void Parser::parse(const vector<string>& input) {
    stack<string> parseStack;

    parseStack.push(END);
    parseStack.push(startSymbol);

    size_t inputIndex = 0;
    derivationSteps.clear();

    while (!parseStack.empty()) {
        string top = parseStack.top();
        parseStack.pop();
        // case if the stack is empty and there remains inputs
        if (top == END) {
            if(inputIndex == input.size() - 1){
                derivationSteps.push_back("accept");
            }else{
                // case if the stack is empty and there remains inputs
                derivationSteps.push_back("reject, Stack is empty");
            }
            continue;
        }

        if (terminals.find(top) != terminals.end()) {
            // terminals
            if (inputIndex < input.size() && input[inputIndex] == top) {
                derivationSteps.push_back("match " + top);
                inputIndex++;
            } else {
                // case if the terminal in the stack does not match the input token action remove from the stack
                derivationSteps.push_back("Error, expected " + top + " but found " + input[inputIndex]);
                parseStack.pop();
            }
        } else {
            // non-terminals
            vector<string> production = parsingTable.getProduction(top, input[inputIndex]);
            // if production sync then error recovery action remove from stack this case should be included
            if (production.empty()) {
                // case of error recovery action remove from the input token action discard the input token
                derivationSteps.push_back("Error:(illegal "+ top +" ), discard " + input[inputIndex] + ")");
                inputIndex++;
            } else {
                derivationSteps.push_back(top + " -> " + join(production, " "));
                for (auto it = production.rbegin(); it != production.rend(); ++it) {
                    if (*it != EPSILON) {
                        parseStack.push(*it);
                    }
                }
            }
        }
    }
    printDerivation();
}

void Parser::printDerivation() const {
    for (const auto& step : derivationSteps) {
        cout << step << endl;
    }
}

