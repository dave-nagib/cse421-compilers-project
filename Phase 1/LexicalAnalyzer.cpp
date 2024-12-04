#include "LexicalAnalyzer.h"
using namespace std;




/** Constructor */
LexicalAnalyzer::LexicalAnalyzer(const string &rules_file_path)
{
  // Initialize the DFA, mapper, and token names
  ifstream rules_file(rules_file_path);
  if (!rules_file.is_open()) {
    throw runtime_error("Could not open the rules file.");
  }
  // use absolute path for Test Illustrations\lexical_rules_test1.txt
  RegexAnalyzer regex_analyzer(rules_file_path);
  NFA nfa = regex_analyzer.RegexToNFA();

  // Use std::unordered_map and std::vector explicitly
  std::unordered_map<int, std::string> tokens = regex_analyzer.getTokensIdNameMap();
  std::unordered_map<char, char> charTokens = regex_analyzer.getCharTokensMap();

  NFA2DFA converter;
  std::vector<char> input_domain;
  for (auto const &pair: charTokens) {
    input_domain.push_back(pair.second);
  }
  DFA dfa = converter.convert(nfa, input_domain);

  DFAMinimizer minimizer(dfa);
  DFA minimized_dfa = minimizer.minimize();
  tokens[-1] = "ERROR";
  unordered_map<char, char> tokenChars;
    for (auto i: charTokens) {
        tokenChars[i.second] = i.first;
    }
  minimized_dfa.print_dfa(tokenChars, tokens);

  // Assign fields
  this->dfa = std::move(minimized_dfa);
  this->token_names = std::move(tokens);
  this->mapper = std::move(charTokens);

}

/** Method to fill the buffer. Returns true if there are no more characters in the input stream. */
bool LexicalAnalyzer::fill_buffer(vector<char> &buffer, ifstream &ip)
{
  char c;
  int counter = 0;
  while (counter < BUFFER_SIZE && ip.get(c))
  {
    // Only add one whitespace character to the buffer
    if (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\v' || c == '\f') {
        if (!buffer.empty() && buffer.back() == ' ') continue;
        c = ' ';
    }
    buffer.push_back(c);
    ++counter;
  }
  return counter == 0;
}


/** Method to analyze input and build the symbol table */
vector<Symbol> LexicalAnalyzer::analyze(ifstream &input_file)
{
  vector<Symbol> symbol_table;            // The symbol table to return
  vector<char> buffer;                    // Buffer to store the current lexeme
  int current_state = dfa.get_initial();  // Start with the initial state.
  int last_token = -1;                    // Track the last accepting state
  int dead_state = this->dfa.get_dead_state();
  int next_state = 0;
  size_t i = 0, end_i = 1;                // Position of current element in the buffer and the last accepting element
  bool eof_flag = false;                  // Flag to indicate end of file (fill_buffer returns false)
  char c;

  do 
  {
    // Reset EOF flag
    eof_flag = false;
    // Check if the buffer is empty
    if (i == buffer.size())
    {
      // Try to fill the buffer and if there is no more input, set the EOF flag
      // which will be equivalent to reaching a dead state.
      eof_flag = fill_buffer(buffer, input_file);
    }

    // Get next state on inputting the current character. 
    // If we reach EOF or a whitespace character, the next state will be the dead state.
    c = (i < buffer.size())? buffer.at(i) : '\0';
    if (eof_flag || c == ' ')
        next_state = dead_state;
    else
        next_state = dfa.transition(current_state, this->mapper.at(c));

    // If this inputs leads to a dead state, this means that there is no further tokens to be found
    // on starting from the current start character. We either accept some token or mark an error.
    if (next_state == dead_state) 
    {
      symbol_table.push_back({
        string(buffer.begin(), buffer.begin() + end_i), // Lexeme, will be one character only if no token was found (end_i = 1)
        this->token_names.at(last_token) // Token name, will be ERROR if no token was found (last token = -1)
      });
      // Delete the lexeme from the buffer
      buffer.erase(buffer.begin(), buffer.begin() + end_i); // Will only erase one character if no token was found
      // Reset last token, end position, current state, and pointer
      last_token = -1;
      end_i = 1;
      current_state = dfa.get_initial();
      i = 0;
      // If there is a whitespace at the beginning of the buffer, remove it
      if (!buffer.empty() && buffer.front() == ' ') buffer.erase(buffer.begin());
      continue;
    }

    // If the next state is accepting, update the last token and end position
    if (dfa.accept(next_state) != -1)
    {
      last_token = dfa.accept(next_state);
      end_i = i+1;
    }

    // increment i and update current state
    ++i;
    current_state = next_state;

  } while (!(buffer.empty() && eof_flag)); // Continue until the buffer is empty and the EOF flag is set

  return symbol_table;
}