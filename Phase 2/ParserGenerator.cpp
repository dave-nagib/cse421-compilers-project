//
// Created by alimedhat on 30/12/2024.
//


# include "ParserGenerator.h"

using namespace std;

void ParserGenerator::generateParser(const vector<string>& input, const string &output_file_path) {
    string derivation_path = output_file_path.substr(0, output_file_path.find_last_of('.')) + "_derivation.txt";
    parser->parse(input, derivation_path);
}

void ParserGenerator::printAll(const string &rules_file_path) {
    string grammar_file_path = rules_file_path.substr(0, rules_file_path.find_last_of('.')) + "_grammar.txt";
    string terminals_file_path = rules_file_path.substr(0, rules_file_path.find_last_of('.')) + "_terminals.txt";
    string non_terminals_file_path = rules_file_path.substr(0, rules_file_path.find_last_of('.')) + "_non_terminals.txt";
    string parsing_table_file_path = rules_file_path.substr(0, rules_file_path.find_last_of('.')) + "_parsing_table.txt";
    parserRulesReader.printGrammar(grammar_file_path);
    parserRulesReader.printTerminals(terminals_file_path);
    parserRulesReader.printNonTerminals(non_terminals_file_path);
    table.printTable(parsing_table_file_path);
}