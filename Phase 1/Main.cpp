#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "LexicalAnalyzer.h"

int main(int argc, char *argv[]) {
    std::string rules_file_path;
    if (argc != 3) {
        std::cout << "Enter the path to the lexical rules file: ";
        std::cin >> rules_file_path;
    } else {
        rules_file_path = argv[1];
    }

   // Initialize the lexical analyzer based on the rules file
    LexicalAnalyzer lexical_analyzer(rules_file_path);

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
