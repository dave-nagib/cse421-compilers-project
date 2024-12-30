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
    // Sets of terminals and non-terminals in the parsing table
    SymbolSet terminals;
    SymbolSet nonTerminals;
    // Table of productions
    std::unordered_map<std::string, std::vector<std::string>> table;

public:
    ParsingTable () = default;

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

    // Add a production to the table
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
