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
    const std::string EPSILON = "\0";
    const std::string END = "$";
    SymbolSet terminals;
    SymbolSet nonTerminals;

    std::string join(const std::vector<std::string>& vec, const std::string& delimiter) {
        std::string result;
        for (const auto& str : vec) {
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
    }

    bool parse(const std::vector<std::string>& input);
    bool panicModeRecovery(const std::string& nonTerminal,
                           const std::string& inputToken,
                           const std::vector<std::string>& input,
                           size_t& inputIndex);
    void printDerivation() const;
};

#endif //DFA_CPP_PARSER_H
