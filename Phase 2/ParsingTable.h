#ifndef DFA_CPP_PARSINGTABLE_H
#define DFA_CPP_PARSINGTABLE_H

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <fstream>
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
        bool exists = table.find(nonTerminal + " " + terminal) != table.end();
        if (exists && table[nonTerminal + " " + terminal] != production) {
//            std::cout << "Production exists: ";
//            for (const auto& sym : table[nonTerminal + " " + terminal]) std::cout << sym << " ";
//            std::cout << "\nTried to add: ";
//            for (const auto& sym : production) std::cout << sym << " ";
            throw std::invalid_argument("\nProduction rule already exists for " + nonTerminal + ", " + terminal);
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

    void printTable(const std::string &non_terminals_file_path){
        std::ofstream output_file(non_terminals_file_path);
        if (!output_file.is_open()) {
            std::cerr << "Error: Could not open file "
                 << non_terminals_file_path
                 << " for writing." << std::endl;
            return;
        }
        output_file << "Parsing Table:" << std::endl;
        for (auto i : table){
            output_file << i.first << " -> ";
            for (auto j : i.second){
                output_file << j << " ";
            }
            output_file << std::endl;
        }
        std::cout << "Parsing Table written to " << non_terminals_file_path << std::endl;
        output_file.close();
    }
};


#endif
