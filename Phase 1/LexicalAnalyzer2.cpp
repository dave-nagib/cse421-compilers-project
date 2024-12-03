#include "LexicalAnalyzer2.h"
using namespace std;

// TODO:
// Keep in mind that the code now returns symbo table with the accepting state id's as the token ID (this needs to change)
// The change will need a way to map between a state id it's matching tokens, then do a tie break if more than one token exists on this state.

// TODO:
// Also, this lexical analyzer just fails when encountering invalid symbols, no recovery.

/** Method to fill the buffer. Returns true if there are no more characters in the input stream. */
bool fill_buffer(std::vector<char> &buffer, std::ifstream &ip)
{
  char c;
  int counter = 0;
  while (counter < BUFFER_SIZE && ip.get(c))
  {
    // Skip whitespace
    if (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\v' || c == '\f') continue;
    buffer.push_back(c);
    ++counter;
  }
  return counter == 0;
}

/** Constructor */
LexicalAnalyzer2::LexicalAnalyzer2(
    const DFA &dfa, 
    const unordered_map<char,char> &mapper,
    const unordered_map<int, string> &token_names
  ) : dfa(dfa), mapper(mapper), token_names(token_names) 
{
  if (token_names.find(-1) != token_names.end() && token_names.at(-1) != "ERROR") 
  {
    throw runtime_error("Token names map must contain an ERROR token.");
  }
  for (auto const &pair : dfa.get_accepting())
  {
    if (token_names.find(pair.second) == token_names.end()) 
    throw runtime_error("Token names map must contain all token IDs in the DFA.");
  }
}

/** Method to analyze input and build the symbol table */
vector<Symbol> LexicalAnalyzer2::analyze(ifstream &input_file)
{
  vector<Symbol> symbol_table;            // The symbol table to return
  vector<char> buffer;                    // Buffer to store the current lexeme
  int current_state = dfa.get_initial();  // Start with the initial state.
  int last_token = -1;                    // Track the last accepting state
  int dead_state = this->dfa.get_dead_state();
  int next_state = 0;
  size_t i = 0, end_i = 1;                // Position of current element in the buffer and the last accepting element
  bool eof_flag = false;                  // Flag to indicate end of file (fill_buffer returns false)

  // // Print the accepting states
  // cout << "Accepting states: ";
  // for (auto const &pair : dfa.get_accepting())
  // {
  //   cout << pair.first << " ";
  // }

  // // print the buffer size
  // cout << "Buffer size: " << buffer.size() << endl;

  do 
  {
    // Check if the buffer is empty
    if (i == buffer.size())
    {
      // Try to fill the buffer and if there is no more input, set the EOF flag
      // which will be equivalent to reaching a dead state.
      eof_flag = fill_buffer(buffer, input_file);
    }

    // Get next state on inputting the current character. 
    // If we reach EOF, the next state will be the dead state.
    next_state = (!eof_flag)? dfa.transition(current_state, this->mapper.at(buffer.at(i))) 
                            : dead_state;
    // print the next state
    //  cout << "Next state: " << next_state << endl;

    // If this inputs leads to a dead state, this means that there is no further tokens to be found
    // on starting from the current start character. We either accept some token or mark an error.
    if (next_state == dead_state) 
    {
      // //print the lexeme
      // cout << "Lexeme: ";
      // for (size_t j = last_accepting_pos; j < end_i; j++)
      // {
      //   cout << buffer.at(j);
      // }
      // Add to the symbol table the last accepted lexeme and token ID
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
      // Reset EOF flag
      eof_flag = false;
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

  } while (buffer.size() > 0);

  // Handle remaining characters
  return symbol_table;
}