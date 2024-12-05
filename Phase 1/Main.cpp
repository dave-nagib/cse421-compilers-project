#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "LexicalAnalyzer.h"

int main(int argc, char *argv[]) {
    std::string rules_file_path, output_file_path;
    if (argc != 4) {
        std::cout << "Enter the path to the lexical rules file: ";
        std::cin >> rules_file_path;
        std::cout << "\nEnter the path to the output file: ";
        std::cin >> output_file_path;
    } else {
        rules_file_path = argv[1];
        output_file_path = argv[2];
    }

   // Initialize the lexical analyzer based on the rules file
    LexicalAnalyzer lexical_analyzer(rules_file_path, output_file_path);
    while (true)
    {
        std::cout << "To exit, type 'exit'." << std::endl;
        std::string input_file_path;
        if (argc != 3) {
            std::cout << "Enter the path to the input file: ";
            std::cin >> input_file_path;
        } else {
            input_file_path = argv[3];
        }
        if (input_file_path == "exit") {
            break;
        }

        std::ifstream input_file(input_file_path, std::ios::binary);
        std::vector<Symbol> symbol_table = lexical_analyzer.analyze(input_file);
        //close the input file
        input_file.close();
        //before the file extension in the input file path add _tokens
        std::string tokens_file_path = input_file_path.substr(0, input_file_path.find_last_of('.')) + "_tokens_SymbolTable.txt";
        //write the tokens to the new tokens file
        std::ofstream tokens_file(tokens_file_path);
        for (const Symbol &symbol : symbol_table) {
            tokens_file << symbol.token_name << std::endl;
        }
        //write the pairs in a table in the same file
        tokens_file << "Symbol Table:" << std::endl;
        tokens_file << std::left << std::setw(10) << "Lexeme" << "Token ID" << std::endl;
        tokens_file << std::string(30, '-') << std::endl; // Separator line
        for (const Symbol &symbol : symbol_table) {
            //write only symbols with a token id = id
            tokens_file << std::left << std::setw(10) << symbol.lexeme << symbol.token_name << std::endl;
        }
        tokens_file.close();
        std::cout << "Tokens written to " << tokens_file_path << std::endl;

    }
    return 0;
}
