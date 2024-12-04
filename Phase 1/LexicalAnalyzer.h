#include <vector>
#include <unordered_map>
#include <string>
#include "RegexAnalyzer.h"
#include "NFA.h"
#include "NFA2DFA.h"
#include "DFA.h"
#include "DFAMinimizer.h"

const int BUFFER_SIZE = 256;

struct Symbol
{
    std::string lexeme; // The lexeme (token)
    std::string token_name;       // Token ID associated with the accepting state
};

class LexicalAnalyzer
{
  private:
    DFA dfa; // The minimized DFA
    std::unordered_map<int, std::string> token_names; // Map from accepting state to token name
    std::unordered_map<char, char> mapper; // Map from character to character ID
    /** Method to fill the buffer. Returns true if there are no more characters in the input stream. */
    static bool fill_buffer(std::vector<char> &buffer, std::ifstream &ip);
  public:
    /** default constructor */
    LexicalAnalyzer();
    LexicalAnalyzer(const std::string& rules_file_path);
    std::vector<Symbol> analyze(std::ifstream &input_file);
};