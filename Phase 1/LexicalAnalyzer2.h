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
    std::string token_name;       // Token ID associated with the accepting state
};

class LexicalAnalyzer2 
{
  private:
    const DFA &dfa; // Reference to the DFA used for analysis
    const std::unordered_map<char, char> &mapper;
    const std::unordered_map<int, std::string> &token_names;
  public:
    LexicalAnalyzer2(
      const DFA &dfa, 
      const std::unordered_map<char,char> &mapper,
      const std::unordered_map<int, std::string> &token_names
    );
    std::vector<Symbol> analyze(std::ifstream &input_file);
};