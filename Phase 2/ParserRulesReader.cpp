//
// Created by Abdulla S.Talha on 12/24/2024.
//

#include "ParserRulesReader.h"
#include <fstream>
#include <iostream>
#include <regex>
#include <utility>

ParserRulesReader::ParserRulesReader() = default;

void ParserRulesReader::readRules(string rulesFilePath) {
    this->rulesFilePath = std::move(rulesFilePath);
    readRulesFile();
    bool notLL1 = generateLL1Grammar();
    if (notLL1) {
        cout << "The grammar is not LL1" << endl;
    }
    else {
        cout << "The grammar is LL1" << endl;
    }


}

void ParserRulesReader::readRulesFile() {
    std::ifstream file(rulesFilePath);
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open file: " + rulesFilePath);
    }

    std::string line;
    std::string currentLHS;
    string currentExpression;
    std::vector<std::string> currentRHS;
    std::vector<std::vector<std::string>> currentRules;
    bool newRule = true;
    bool isLHSSide = true;
    bool isRHSSide = false;
    bool isTerminal = false;
    // # means new rule
    while (std::getline(file, line)) {
        for (int i = 0; i < line.size(); i++) {
            char c = line[i];
            if (newRule) {
                currentLHS = "";
                currentRules.clear();
                newRule = false;
                isLHSSide = true;
                isRHSSide = false;
                isTerminal = false;
                if (c == '#') {
                    continue;
                }
                --i;
            }
            else if (isLHSSide) {
                if (c == ' ') {
                    continue;
                } else if (c == ':') {
                    isLHSSide = false;
                    isRHSSide = true;
                    i += 2;
                    if (nonTerminals.empty()) {
                        startingSymbol = currentLHS;
                    }
                    nonTerminals.insert(currentLHS);
                } else {
                    currentLHS += c;
                }
            }
            else if (isRHSSide) {
                if (c == ' '){
                    if (!currentExpression.empty()) {
                        currentRHS.push_back(currentExpression);
                        nonTerminals.insert(currentExpression);
                        currentExpression = "";
                    }
                }else if (c == '|') {
                    if (!currentExpression.empty()) {
                        currentRHS.push_back(currentExpression);
                        nonTerminals.insert(currentExpression);
                        currentExpression = "";
                    }
                    currentRules.push_back(currentRHS);
                    currentRHS.clear();
                } else if (c == '#') {
                    if (!currentExpression.empty()) {
                        currentRHS.push_back(currentExpression);
                        nonTerminals.insert(currentExpression);
                        currentExpression = "";
                    }
                    currentRules.push_back(currentRHS);
                    grammar[currentLHS] = currentRules;
                    currentRHS.clear();
                    newRule = true;
                    isLHSSide = false;
                    isRHSSide = false;
                } else if (c == '\'') {
                    if (isTerminal) {
                        currentRHS.push_back(currentExpression);
                        terminals.insert(currentExpression);
                        currentExpression = "";
                        isTerminal = false;
                    } else {
                        isTerminal = true;
                    }
                } else if (c == '\n') {
                    if (!currentExpression.empty()) {
                        currentRHS.push_back(currentExpression);
                        nonTerminals.insert(currentExpression);
                        currentExpression = "";
                    }
                } else if (c == '\\') {
                    if (line[i + 1] == 'L') {
                        if (!currentExpression.empty()) {
                            currentRHS.push_back(currentExpression);
                            currentExpression = "";
                        }
                        currentRHS.emplace_back("\\L");
                        terminals.insert("\\L");
                        i += 1;
                    }
                }
                else {
                    currentExpression += c;
                }
            }
        }
    }
    //add the last rule
    if (!currentExpression.empty()) {
        currentRHS.push_back(currentExpression);
        nonTerminals.insert(currentExpression);
        currentExpression = "";
    }
    currentRules.push_back(currentRHS);
    grammar[currentLHS] = currentRules;
}


bool ParserRulesReader::generateLL1Grammar() {
    bool flag1 = eliminateLeftRecursion();
    bool flag2 = applyLeftFactoring();
    return flag1 || flag2;
}

bool ParserRulesReader::eliminateLeftRecursion() {
    bool notLL1 = false;

    for (auto& [lhs, rules] : grammar) {
        std::vector<std::vector<std::string>> newRules;
        std::vector<std::vector<std::string>> leftRecursiveRules;

        // Separate left-recursive and non-left-recursive rules
        for (auto& rule : rules) {
            if (!rule.empty() && rule[0] == lhs) {
                leftRecursiveRules.push_back(std::vector<std::string>(rule.begin() + 1, rule.end())); // Remove `lhs`
            } else {
                newRules.push_back(rule);
            }
        }

        if (!leftRecursiveRules.empty()) {
            notLL1 = true;

            // Create a new non-terminal for left recursion
            std::string newNonTerminal = lhs + "'";
            nonTerminals.insert(newNonTerminal);

            // Update the original non-left-recursive rules
            for (auto& rule : newRules) {
                rule.push_back(newNonTerminal);
            }

            // Add new rules for the new non-terminal
            std::vector<std::vector<std::string>> newNonTerminalRules;
            for (auto& rule : leftRecursiveRules) {
                rule.push_back(newNonTerminal);
                newNonTerminalRules.push_back(rule);
            }
            newNonTerminalRules.push_back({"\\L"}); // Add epsilon (lambda)

            // Update the grammar
            grammar[lhs] = newRules;
            grammar[newNonTerminal] = newNonTerminalRules;
        }
    }

    return notLL1;
}


bool ParserRulesReader::applyLeftFactoring() {
    bool notLL1 = false;

    for (auto& [lhs, rules] : grammar) {
        std::unordered_map<std::string, std::vector<std::vector<std::string>>> prefixGroups;
        std::vector<std::vector<std::string>> factoredRules;

        // Group rules by their common prefixes
        for (auto& rule : rules) {
            std::string prefix = rule.empty() ? "" : rule[0];
            prefixGroups[prefix].push_back(rule);
        }

        // Process each group
        for (auto& [prefix, group] : prefixGroups) {
            if (group.size() > 1) {
                notLL1 = true;

                // Create a new non-terminal
                std::string newNonTerminal = lhs + "'";
                nonTerminals.insert(newNonTerminal);

                // Add the factored rule to the original non-terminal
                factoredRules.push_back({prefix, newNonTerminal});

                // Add new rules for the new non-terminal
                std::vector<std::vector<std::string>> newRules;
                for (auto& rule : group) {
                    if (rule.size() > 1) {
                        newRules.push_back(std::vector<std::string>(rule.begin() + 1, rule.end())); // Remove the prefix
                    } else {
                        newRules.push_back({"\\L"}); // Handle epsilon
                    }
                }

                grammar[newNonTerminal] = newRules;
            } else {
                factoredRules.push_back(group[0]); // Single rule, no factoring needed
            }
        }

        // Update the grammar
        grammar[lhs] = factoredRules;
    }

    return notLL1;
}


void ParserRulesReader::printGrammar() {
    cout << "Start Symbol: " << startingSymbol << endl;
    cout << "Grammar:" << endl;
    for (auto const&[key, val] : grammar) {
        cout << key << " -> ";
        for (auto const& rule : val) {
            for (auto const& symbol : rule) {
                cout << symbol << " ";
            }
            cout << "| ";
        }
        cout << endl;
    }
}

void ParserRulesReader::printTerminals() {
    cout << "Terminals: " << endl;
    for (auto const& terminal : terminals) {
        cout << terminal << endl;
    }
}

void ParserRulesReader::printNonTerminals() {
    cout << "Non-Terminals: " << endl;
    for (auto const& nonTerminal : nonTerminals) {
        cout << nonTerminal << endl;
    }
}

Grammar ParserRulesReader::getGrammar() {
    return grammar;
}

SymbolSet ParserRulesReader::getTerminals() {
    return terminals;
}

SymbolSet ParserRulesReader::getNonTerminals() {
    return nonTerminals;
}

string ParserRulesReader::getStartingSymbol() {
    return startingSymbol;
}