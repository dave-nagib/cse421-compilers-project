//
// Created by alimedhat on 28/12/2024.
//

#ifndef DFA_CPP_PARSER_H
#define DFA_CPP_PARSER_H


#include <string>
#include "ParsingTable.h"
#include "ParsingTableGenerator.h"

class Parser {
private:
    ParsingTable parsingTable;
    std::string startSymbol;
    std::vector<std::string> derivationSteps;
    std::unordered_set<std::string> synchronizationPoints = {";", "}", "$"};
    std::string EPSILON = "\0";
    std::string END = "$";
    std::string SYNCH = "\\SYNCH";
    SymbolSet terminals;
    SymbolSet nonTerminals;

    std::string join(const std::vector<std::string>& vec, const std::string& delimiter) {
        std::string result;
        for (const auto& str : vec) {
            if(str == EPSILON) {
                result += "\\L" + delimiter;
                continue;
            }
            result += str + delimiter;
        }
        return result;
    }

public:
    Parser(ParsingTable table, std::string startSym, SymbolSet terms, SymbolSet nonTerms)
            : parsingTable(std::move(table)), startSymbol(std::move(startSym)),
              terminals(std::move(terms)), nonTerminals(std::move(nonTerms)) {}

    Parser(ParsingTableGenerator tableGenerator){
        parsingTable = std::move(tableGenerator.getTable());
        startSymbol = std::move(tableGenerator.getStartSym());
        terminals = std::move(tableGenerator.getTerminals());
        nonTerminals = std::move(tableGenerator.getNonTerminals());
        EPSILON = ParsingTableGenerator::EPSILON;
        END = ParsingTableGenerator::END;
        SYNCH = ParsingTableGenerator::SYNCH;
    }

    void parse(const std::vector<std::string>& input);
    void printDerivation() const;
};

#endif //DFA_CPP_PARSER_H
