#include "RegexAnalyzer.h"
#include "NFA.h"
#include "NFA2DFA.h"
#include "DFA.h"
#include "DFAMinimizer.h"
#include "LexicalAnalyzer2.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <stdexcept>
#include <unordered_map>
#include <string>

std::vector<char> readFileToVector(const std::string& input_file_path) {
    std::ifstream input_file(input_file_path, std::ios::binary);
    if (!input_file.is_open()) {
        throw std::runtime_error("Error opening file: " + input_file_path);
    }
    input_file.seekg(0, std::ios::end);
    std::streamsize file_size = input_file.tellg();
    input_file.seekg(0, std::ios::beg);

    std::vector<char> file_contents(file_size);
    if (!input_file.read(file_contents.data(), file_size)) {
        throw std::runtime_error("Error reading file: " + input_file_path);
    }
    input_file.close();
    return file_contents;
}

std::vector<char> cleanVector(const std::vector<char>& input_file) {
    std::vector<char> cleaned_file;
    for (char c : input_file) {
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\v' || c == '\f') continue;
        cleaned_file.push_back(c);
    }
    return cleaned_file;
}

int main(int argc, char *argv[]) {
    std::string rules_file_path;
    std::cout << "Enter the path to the lexical rules file: ";
    std::cin >> rules_file_path;

    RegexAnalyzer regex_analyzer("./test/Test Illustrations/lexical_rules_test1.txt");
    NFA nfa = regex_analyzer.RegexToNFA();

    // Use std::unordered_map and std::vector explicitly
    std::unordered_map<char, std::string> tokens = regex_analyzer.getTokensIdNameMap();
    std::unordered_map<char, char> charTokens = regex_analyzer.getCharTokensMap();

    NFA2DFA converter;
    std::vector<char> input_domain;
    for (auto const &pair : charTokens) {
        input_domain.push_back(pair.second);
    }
    DFA dfa = converter.convert(nfa, input_domain);

    DFAMinimizer minimizer(dfa);
    DFA minimized_dfa = minimizer.minimize();

    LexicalAnalyzer2 lexical_analyzer(minimized_dfa, charTokens);

    std::string input_file_path;
    std::cout << "Enter the path to the input file: ";
    std::cin >> input_file_path;

    std::vector<char> input_file = readFileToVector("./test/Test Illustrations/sample_program_test1.txt");
    input_file = cleanVector(input_file);

    std::vector<Symbol> symbol_table = lexical_analyzer.analyze(input_file);

    std::cout << "Symbol Table:" << std::endl;
    for (const Symbol &symbol : symbol_table) {
        std::cout << "Lexeme: " << symbol.lexeme << ", Token ID: " << symbol.token_id << std::endl;
    }

    std::cout << "Tokens Names:" << std::endl;
    for (const Symbol &symbol : symbol_table) {
        std::cout << tokens[symbol.token_id] << std::endl;
    }
    return 0;
}
