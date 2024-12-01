#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include "DFA.h"

const int BUFFER_SIZE = 256;

struct Symbol
{
    std::string lexeme; // The lexeme (token)
    int token_id;       // Token ID associated with the accepting state
};

class LexicalAnalyzer2 
{
  private:
    const DFA &dfa; // Reference to the DFA used for analysis
    const std::unordered_map<char, char> mapper;
  public:
    LexicalAnalyzer2(const DFA &dfa, const std::unordered_map<char,char> &mapper) : dfa(dfa), mapper(mapper) {}
    std::vector<Symbol> analyze(std::vector<char> &input_file);
};