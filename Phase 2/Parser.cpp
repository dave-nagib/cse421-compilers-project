//
// Created by alimedhat on 28/12/2024.
//


#include <iostream>
#include <stack>
#include "Parser.h"

using namespace std;

bool Parser::parse(const vector<string>& input) {
    stack<string> parseStack;

    parseStack.push(END);
    parseStack.push(startSymbol);

    size_t inputIndex = 0;
    derivationSteps.clear();
    derivationSteps.push_back(startSymbol);

    while (!parseStack.empty()) {
        string top = parseStack.top();
        parseStack.pop();

        if (top == END) {
            return inputIndex == input.size();
        }

        if (terminals.find(top) != terminals.end()) {
            if (inputIndex < input.size() && input[inputIndex] == top) {
                inputIndex++;
            } else {
                if (!panicModeRecovery(top, input[inputIndex], input, inputIndex)) {
                    return false;
                }
            }
        } else {
            vector<string> production = parsingTable.getProduction(top, input[inputIndex]);
            if (production.empty()) {
                if (!panicModeRecovery(top, input[inputIndex], input, inputIndex)) {
                    return false;
                }
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
    return inputIndex == input.size();
}


bool Parser::panicModeRecovery(const string& nonTerminal,
                               const string& inputToken,
                               const vector<string>& input,
                               size_t& inputIndex) {
    cerr << "Error: Unexpected token '" << inputToken << "' when parsing '" << nonTerminal << "'.\n";
    cerr << "Attempting to recover...\n";

    // Skip tokens until a synchronization point is found
    while (inputIndex < input.size() &&
            synchronizationPoints.find(input[inputIndex]) == synchronizationPoints.end()) {
        inputIndex++;
    }

    // If end of input reached, report and terminate
    if (inputIndex == input.size()) {
        cerr << "Error: Unable to recover. End of input reached.\n";
        return false;
    }

    cerr << "Recovery successful. Resuming at token '" << input[inputIndex] << "'.\n";
    return true;
}

void Parser::printDerivation() const {
    for (const auto& step : derivationSteps) {
        cout << step << endl;
    }
}

