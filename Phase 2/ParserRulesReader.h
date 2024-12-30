//
// Created by Abdulla S.Talha on 12/24/2024.
//

#ifndef PARSERRULESREADER_H
#define PARSERRULESREADER_H
#include "ParsingDataStructs.h"
#include <string>
using namespace std;


class ParserRulesReader {
private:
    Grammar grammar;
    SymbolSet terminals;
    SymbolSet nonTerminals;
    string rulesFilePath;
    string startingSymbol;
    char order = '0';
    void readRulesFile();
    bool generateLL1Grammar();
    bool eliminateLeftRecursion();
    bool applyLeftFactoring();
    static string findLongestCommonPrefix(const std::vector<std::vector<std::string>>& rules);

public:
    /** Default constructor */
    explicit ParserRulesReader();
    /** Reads the rules from the file and generates the grammar and the terminals and non-terminals */
    void readRules(string rulesFilePath);
    /** Prints the grammar */
    void printGrammar();
    /** Prints the terminals */
    void printTerminals();
    /** Prints the non-terminals */
    void printNonTerminals();

    /** Returns the grammar (const reference) */
    const Grammar& getGrammar() const;

    /** Returns the terminals (const reference) */
    const SymbolSet& getTerminals() const;

    /** Returns the non-terminals (const reference) */
    const SymbolSet& getNonTerminals() const;

    /** Returns the starting symbol (const) */
    const string& getStartingSymbol() const;
};




#endif //PARSERRULESREADER_H
