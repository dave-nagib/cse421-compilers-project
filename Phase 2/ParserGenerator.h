//
// Created by alimedhat on 30/12/2024.
//

#ifndef DFA_CPP_PARSERGENERATOR_H
#define DFA_CPP_PARSERGENERATOR_H

#include "ParserRulesReader.h"
#include "ParsingTableGenerator.h"
#include "Parser.h"

class ParserGenerator {
private:
    ParserRulesReader parserRulesReader;
    ParsingTableGenerator* generator;
    ParsingTable table;
    Parser* parser;

public:
    ParserGenerator(const std::string &rules_file_path){
        parserRulesReader.readRules(rules_file_path);
        generator = new ParsingTableGenerator(parserRulesReader.getGrammar(),
                                               parserRulesReader.getTerminals(),
                                               parserRulesReader.getNonTerminals(),
                                           parserRulesReader.getStartingSymbol());
        table = generator->getTable();
        parser = new Parser(*generator);
    }
    void generateParser(const std::vector<string>& input, const std::string &derivation_path);
    void printAll(const std::string &derivation_path);
};




#endif //DFA_CPP_PARSERGENERATOR_H
