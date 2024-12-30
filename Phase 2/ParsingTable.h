#ifndef DFA_CPP_PARSINGTABLE_H
#define DFA_CPP_PARSINGTABLE_H

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <stdexcept>
#include <iostream>
#include "ParsingDataStructs.h"

class ParsingTable {
private:
    // Sets of terminals and non-terminals mapped to productions in the parsing table
//    std::unordered_map<std::string, int> terminals;
//    std::unordered_map<std::string, int> nonTerminals;
    SymbolSet terminals;
    SymbolSet nonTerminals;
    // Dynamic 2D array of productions
    //std::vector<std::string>** table;
    std::unordered_map<std::string, std::vector<std::string>> table;

public:
    ParsingTable() {
        //table = nullptr;
    }

    // Check if table is initialized
//    bool isInitialized() const {
//        return table != nullptr;
//    }

    // Get the production for a given non-terminal and terminal
//    std::vector<std::string> getProduction(const std::string& nonTerminal, const std::string& terminal) const {
//        return table[nonTerminals.at(nonTerminal)][terminals.at(terminal)];
//    }
    std::vector<std::string> getProduction(const std::string& nonTerminal, const std::string& terminal) const {
        std::string key = nonTerminal + " " + terminal;

        // Check if the key exists in the table
        if (table.find(key) != table.end()) {
            return table.at(key);
        } else {
            // Handle the case where the key does not exist
            return {}; // Return an empty vector
        }
    }

    // Initialize the table with given dimensions
//    void initializeTable(size_t nonTerminalCount, size_t terminalCount) {
//        table = new std::vector<std::string>*[nonTerminalCount];
//        for (size_t i = 0; i < nonTerminalCount; i++) {
//            table[i] = new std::vector<std::string>[terminalCount];
//        }
//    }

    // Add a production to the table
//    void addProduction(const std::string& nonTerminal, const std::string& terminal, const std::vector<std::string>& production) {
//        // Throw an error if a production rule already exists
//        if (!table[nonTerminals.at(nonTerminal)][terminals.at(terminal)].empty()) {
//            throw std::invalid_argument("Production rule already exists");
//        }
//        table[nonTerminals.at(nonTerminal)][terminals.at(terminal)] = production;
//    }
    void addProduction(const std::string& nonTerminal, const std::string& terminal, const std::vector<std::string>& production) {
        // Throw an error if a production rule already exists
        if (table.find(nonTerminal +" "+ terminal) != table.end() ) {
            throw std::invalid_argument("Production rule already exists");
        }
        table[nonTerminal + " " + terminal] = production;
    }
    void printTable(){
        for (auto i : table){
            std::cout << i.first << " -> ";
            for (auto j : i.second){
                std::cout << j << " ";
            }
            std::cout << std::endl;
        }
    }
};


#endif
