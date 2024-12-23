#ifndef DFA_CPP_PARSINGTABLE_H
#define DFA_CPP_PARSINGTABLE_H

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <stdexcept>
#include "ParsingDataStructs.h"

class ParsingTable {
private:
    // Sets of terminals and non-terminals mapped to productions in the parsing table
    std::unordered_map<std::string, int> terminals;
    std::unordered_map<std::string, int> nonTerminals;
    // Dynamic 2D array of productions
    std::vector<std::string>** table;

public:
    ParsingTable() {
        table = nullptr;
    }

    // Check if table is initialized
    bool isInitialized() const {
        return table != nullptr;
    }

    // Get the production for a given non-terminal and terminal
    std::vector<std::string> getProduction(const std::string& nonTerminal, const std::string& terminal) const {
        return table[nonTerminals.at(nonTerminal)][terminals.at(terminal)];
    }

    // Initialize the table with given dimensions
    void initializeTable(size_t nonTerminalCount, size_t terminalCount) {
        table = new std::vector<std::string>*[nonTerminalCount];
        for (size_t i = 0; i < nonTerminalCount; i++) {
            table[i] = new std::vector<std::string>[terminalCount];
        }
    }

    // Add a production to the table
    void addProduction(const std::string& nonTerminal, const std::string& terminal, const std::vector<std::string>& production) {
        // Throw an error if a production rule already exists
        if (!table[nonTerminals.at(nonTerminal)][terminals.at(terminal)].empty()) {
            throw std::invalid_argument("Production rule already exists");
        }
        table[nonTerminals.at(nonTerminal)][terminals.at(terminal)] = production;
    }
};


#endif
