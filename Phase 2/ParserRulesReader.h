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
    void readRulesFile();
    bool generateLL1Grammar();
    bool eliminateLeftRecursion();
    bool applyLeftFactoring();

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
    /** Returns the grammar */
    Grammar getGrammar();
    /** Returns the terminals */
    SymbolSet getTerminals();
    /** Returns the non-terminals */
    SymbolSet getNonTerminals();
    /** Returns the starting symbol */
    string getStartingSymbol();



};



#endif //PARSERRULESREADER_H
