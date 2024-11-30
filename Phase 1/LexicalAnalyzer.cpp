#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include "DFA.h"

// TODO:
// Keep in mind that the code now returns symbo table with the accepting state id's as the token ID (this needs to change)
// The change will need a way to map between a state id it's matching tokens, then do a tie break if more than one token exists on this state.

// TODO:
// Also, this lexical analyzer just fails when encountering invalid symbols, no recovery.

class LexicalAnalyzer
{
public:
  struct Symbol
  {
    std::string lexeme; // The lexeme (token)
    int token_id;       // Token ID associated with the accepting state
  };

  // Constructor
  LexicalAnalyzer(const DFA &dfa) : dfa(dfa) {}

  // Method to analyze input and build the symbol table
  std::vector<Symbol> analyze(const std::string &input)
  {
    std::vector<Symbol> symbol_table;      // The symbol table to return
    std::string buffer;                    // Buffer to store the current lexeme
    int current_state = dfa.get_initial(); // Start with the initial state
    int last_accepting_state = -1;         // Track the last accepting state
    size_t last_accepting_pos = 0;         // Position of the last accepting state

    size_t i = 0;
    while (i < input.size())
    {
      char c = input.at(i);

      // If the character is not part of the DFA's domain or no transition exists
      if (dfa.transition(current_state, c) == -1 || dfa.get_transitions_from(current_state).find(c) == dfa.get_transitions_from(current_state).end())
      {
        // If we encountered an accepting state previously
        if (last_accepting_state != -1)
        {
          // Add the lexeme to the symbol table
          symbol_table.push_back({buffer.substr(0, last_accepting_pos), dfa.get_accepting().at(last_accepting_state)});
          // Reset for the next token
          buffer = buffer.substr(last_accepting_pos - 1);
          i -= (buffer.size() - 1); // Rewind to reprocess the remainder
          buffer.clear();
          current_state = dfa.get_initial();
          last_accepting_state = -1;
        }
        else
        {
          // No accepting state found, invalid token
          throw std::runtime_error("Lexical error: Invalid token encountered at position " + std::to_string(i));
        }
      }
      else
      {
        // Transition exists; process the character
        buffer += c;
        current_state = dfa.get_transitions_from(current_state).at(c);
        if (dfa.get_accepting().find(current_state) != dfa.get_accepting().end())
        {
          last_accepting_state = current_state;
          last_accepting_pos = buffer.size();
        }
        ++i;
      }
    }

    // Process any remaining buffer after the end of the input
    if (last_accepting_state != -1)
    {
      symbol_table.push_back({buffer.substr(0, last_accepting_pos), dfa.get_accepting().at(last_accepting_state)});
    }
    else if (!buffer.empty())
    {
      throw std::runtime_error("Lexical error: Invalid token encountered at the end of the input");
    }

    return symbol_table;
  }

private:
  const DFA &dfa; // Reference to the DFA used for analysis
};

// Example usage.
// This example has a language that accepts 'aab' and 'abc' as tokens
int main()
{
    // Define the DFA
    std::vector<char> domain = {'a', 'b', 'c'};
    int start_state = 0;

    DFA dfa(domain);

    // Add states
    dfa.add_state(0); // Start state
    dfa.add_state(1); // After 'a'
    dfa.add_state(2); // After 'ab'
    dfa.add_state(3); // After 'abc' (Accepting state for "abc")
    dfa.add_state(4); // After 'aa'
    dfa.add_state(5); // After 'aab' (Accepting state for "aab")
    dfa.make_initial(0);

    // Add transitions for "abc"
    dfa.add_transition(0, 'a', 1);
    dfa.add_transition(1, 'b', 2);
    dfa.add_transition(2, 'c', 3);

    // Add transitions for "aab"
    dfa.add_transition(1, 'a', 4);
    dfa.add_transition(4, 'b', 5);

    // Mark accepting states
    dfa.make_accepting(3, 100); // Token ID for "abc"
    dfa.make_accepting(5, 200); // Token ID for "aab"

    // Create LexicalAnalyzer
    LexicalAnalyzer analyzer(dfa);

    // Input string
    std::string input = "abcaababc";

    try
    {
        // Analyze the input
        auto symbol_table = analyzer.analyze(input);

        // Output the symbol table
        for (const auto &symbol : symbol_table)
        {
            std::cout << "Lexeme: " << symbol.lexeme << ", Token ID: " << symbol.token_id << std::endl;
        }
    }
    catch (const std::runtime_error &e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
