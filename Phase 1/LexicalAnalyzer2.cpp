#include "LexicalAnalyzer2.h"

// TODO:
// Keep in mind that the code now returns symbo table with the accepting state id's as the token ID (this needs to change)
// The change will need a way to map between a state id it's matching tokens, then do a tie break if more than one token exists on this state.

// TODO:
// Also, this lexical analyzer just fails when encountering invalid symbols, no recovery.

/** Method to analyze input and build the symbol table */
std::vector<Symbol> LexicalAnalyzer2::analyze(std::vector<char> &input_file)
{
  std::vector<Symbol> symbol_table;      // The symbol table to return
  std::vector<char> buffer = input_file;              // Buffer to store the current lexeme
  int current_state = dfa.get_initial(); // Start with the initial state.
  int last_token = -1, end_i = 1;         // Track the last accepting state
  int dead_state = 0;
  int next_state = 0;
  size_t last_accepting_pos = 0;         // Position of the last accepting state
  char c;                                // Character to read
  size_t i = 0;                          // Position in the buffer

  // Print the accepting states
  std::cout << "Accepting states: ";
  for (auto const &pair : dfa.get_accepting())
  {
    std::cout << pair.first << " ";
  }
  // print the buffer size
  std::cout << "Buffer size: " << buffer.size() << std::endl;
  while (i < buffer.size())
  {
    // Get the current character
    c = buffer.at(i);

    // Get next state
    next_state = dfa.transition(current_state, this->mapper.at(c));
    //print the next state
    std::cout << "Next state: " << next_state << std::endl;

    // If this inputs leads to a dead state, this means that there is no further tokens to be found
    // on starting from the current start character. We either accept some token or mark an error.
    if (next_state == dead_state) 
    {
      //print the lexeme
      std::cout << "Lexeme: ";
      for (size_t j = last_accepting_pos; j < end_i; j++)
      {
        std::cout << buffer.at(j);
      }
      // Add to the symbol table the last accepted lexeme and token ID
      symbol_table.push_back({
        std::string(buffer.begin(), buffer.begin() + end_i), // Lexeme, will be one character only if no token was found (end_i = 1)
        last_token // Will be -1 if no token was found
      });
      // Delete the lexeme from the buffer
      buffer.erase(buffer.begin(), buffer.begin() + end_i); // Will only erase one character if no token was found
      // Reset last token, end position, current state, and pointer
      last_token = -1;
      end_i = 1;
      current_state = dfa.get_initial();
      i = 0;
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
  }

  return symbol_table;
}