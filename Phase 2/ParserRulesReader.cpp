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
                if (c == ' ' || c == '\t') {
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
                if (c == ' ' || c == '\t') {
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
                        if (!isTerminal) {
                            nonTerminals.insert(currentExpression);
                        }
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
    //create a copy of the grammar
    Grammar originalGrammar = grammar;

    for (auto& entry : originalGrammar) {
        auto& lhs = entry.first;
        auto& rules = entry.second;
        std::vector<std::vector<std::string>> newRules;
        std::vector<std::vector<std::string>> nonLeftRecursiveRules;
        std::vector<std::vector<std::string>> leftRecursiveRules;
        // Separate left-recursive and non-left-recursive rules
        for (auto& rule : rules) {
            if (!rule.empty() && rule[0] == lhs) {
                // Left-recursive rule: remove `lhs` from the rule
                leftRecursiveRules.push_back(std::vector<std::string>(rule.begin() + 1, rule.end()));
            } else {
                // Non-left-recursive rule
                nonLeftRecursiveRules.push_back(rule);
            }
        }

        if (!leftRecursiveRules.empty()) {
            notLL1 = true;

            // Create a new non-terminal for left recursion
            std::string newNonTerminal = lhs + order++;
            nonTerminals.insert(newNonTerminal);

            // Update the non-left-recursive rules to use the new non-terminal
            for (auto& rule : nonLeftRecursiveRules) {
                rule.push_back(newNonTerminal);
            }

            // Create new rules for the new non-terminal
            std::vector<std::vector<std::string>> newNonTerminalRules;
            for (auto& rule : leftRecursiveRules) {
                rule.push_back(newNonTerminal);  // Append the new non-terminal
                newNonTerminalRules.push_back(rule);
            }
            newNonTerminalRules.push_back({"\\L"});  // Add epsilon (λ) to terminate recursion

            // Update the grammar
            grammar[lhs] = nonLeftRecursiveRules;
            grammar[newNonTerminal] = newNonTerminalRules;
        }
    }

    return notLL1;
}



bool ParserRulesReader::applyLeftFactoring() {
    bool notLL1 = false;
    Grammar originalGrammar = grammar;

    for (auto& entry : originalGrammar) {
        auto& lhs = entry.first;
        auto& rules = entry.second;

        std::unordered_map<std::string, std::vector<std::vector<std::string>>> prefixGroups;
        std::vector<std::vector<std::string>> factoredRules;

        // Group rules by their common prefixes
        for (auto& rule : rules) {
            if (rule.empty()) continue;
            std::string prefix = findLongestCommonPrefix({rule});  // Use the helper function
            prefixGroups[prefix].push_back(rule);
        }

        // Process each group
        for (auto& prefixGroup : prefixGroups) {
            auto& prefix = prefixGroup.first;
            auto& group = prefixGroup.second;

            if (group.size() > 1) {
                notLL1 = true;

                // Create a new non-terminal for the group
                std::string newNonTerminal = lhs + order++;
                nonTerminals.insert(newNonTerminal);

                // Add the factored rule to the original non-terminal
                factoredRules.push_back({prefix, newNonTerminal});

                // Create new rules for the new non-terminal
                std::vector<std::vector<std::string>> newRules;
                for (auto& rule : group) {
                    if (rule.size() > 1) {
                        newRules.push_back(std::vector<std::string>(rule.begin() + 1, rule.end()));  // Remove the prefix
                    } else {
                        newRules.push_back({"\\L"});  // Handle epsilon
                    }
                }

                grammar[newNonTerminal] = newRules;
            } else {
                factoredRules.push_back(group[0]);  // Single rule, no factoring needed
            }
        }

        // Update the grammar
        grammar[lhs] = factoredRules;
    }

    return notLL1;
}

std::string ParserRulesReader::findLongestCommonPrefix(const std::vector<std::vector<std::string>>& rules) {
    if (rules.empty()) return "";
    std::string prefix = rules[0][0];

    for (const auto& rule : rules) {
        std::string currentPrefix;
        for (size_t i = 0; i < std::min(prefix.size(), rule[0].size()); ++i) {
            if (prefix[i] == rule[0][i]) {
                currentPrefix += prefix[i];
            } else {
                break;
            }
        }
        prefix = currentPrefix;
        if (prefix.empty()) break;
    }

    return prefix;
}


void ParserRulesReader::printGrammar() {
    cout << "Start Symbol: " << startingSymbol << endl;
    cout << "Grammar:" << endl;
    for (auto const& entry : grammar) {
        cout << entry.first << " -> ";
        for (auto const& rule : entry.second) {
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

const Grammar& ParserRulesReader::getGrammar() const {
    return grammar;
}

const SymbolSet& ParserRulesReader::getTerminals() const {
    return terminals;
}

const SymbolSet& ParserRulesReader::getNonTerminals() const {
    return nonTerminals;
}

const string& ParserRulesReader::getStartingSymbol() const {
    return startingSymbol;
}
