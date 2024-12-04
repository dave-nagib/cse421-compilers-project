#include "RegexAnalyzer.h"
#include "NFA.h"
#include "NFA2DFA.h"
#include "DFA.h"
#include "DFAMinimizer.h"
#include "LexicalAnalyzer.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <string>

int main(int argc, char *argv[]) {
    std::string rules_file_path;
    if (argc != 3) {
        std::cout << "Enter the path to the lexical rules file: ";
        std::cin >> rules_file_path;
    } else {
        rules_file_path = argv[1];
    }

    // use absolute path for Test Illustrations\lexical_rules_test1.txt
    RegexAnalyzer regex_analyzer(rules_file_path);
    NFA nfa = regex_analyzer.RegexToNFA();

    // Use std::unordered_map and std::vector explicitly
    std::unordered_map<int, std::string> tokens = regex_analyzer.getTokensIdNameMap();
    std::unordered_map<char, char> charTokens = regex_analyzer.getCharTokensMap();

    NFA2DFA converter;
    std::vector<char> input_domain;
    for (auto const &pair : charTokens) {
        input_domain.push_back(pair.second);
    }
    DFA dfa = converter.convert(nfa, input_domain);

    DFAMinimizer minimizer(dfa);
    DFA minimized_dfa = minimizer.minimize();
    std::cout << "\n";
    //minimized_dfa.print_dfa();

    tokens[-1] = "ERROR";
    LexicalAnalyzer lexical_analyzer(minimized_dfa, charTokens, tokens);

    std::string input_file_path;
    if (argc != 3) {
        std::cout << "Enter the path to the input file: ";
        std::cin >> input_file_path;
    } else {
        input_file_path = argv[2];
    }

    std::ifstream input_file(input_file_path, std::ios::binary);
    std::vector<Symbol> symbol_table = lexical_analyzer.analyze(input_file);

    std::cout << "Symbol Table:" << std::endl;
    for (const Symbol &symbol : symbol_table) {
        std::cout << "Lexeme: " << symbol.lexeme << ", Token ID: " << symbol.token_name << std::endl;
    }
    return 0;
}
