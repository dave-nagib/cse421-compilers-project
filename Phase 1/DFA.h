#ifndef DFA_H
#define DFA_H
#include<unordered_map>
#include<unordered_set>
#include<bits/stdc++.h>


class DFA {
  private:
    std::vector<char> input_domain;
    std::unordered_set<int> states;
    std::unordered_map<int, std::unordered_map<char, int>> transitions;
    int initial_state;
    std::unordered_map<int, int> accepting_states;
    // /** Returns a map of all unique state pairs to a boolean indicating distinguishability */
    // map<pair<int, int>, bool> distinguish_states() const; 
    // /** Rebuilds the current DFA based on some input partition / mapping of old state ID -> new state ID */
    // DFA partition_dfa(std::unordered_map<int, int>& partition) const; 
  public:
    /** Default constructor */
    DFA(std::vector<char> input_domain);
    /** DI constructor */
    DFA(
      std::vector<char> input_domain, 
      std::unordered_set<int> states, 
      std::unordered_map<int, 
      std::unordered_map<char, int>> transitions, 
      int initial_state, 
      std::unordered_map<int, int> accepting_states
    );
    
    /** Returns the input domain */
    std::vector<char> get_input_domain() const;
    /** Returns the initial state */
    int get_initial() const;
    /** Returns the set of states of the DFA. */
    std::unordered_set<int> get_states() const;
    /** Returns a map of accepting states and their corresponding token ids. */
    std::unordered_map<int, int> get_accepting() const;
    /** Returns the transitions out of some state */
    std::unordered_map<char, int> get_transitions_from(int state) const;
    
    /** Add a state to the DFA. The state can be an initial and/or accepting. If it is accepting, a pattern_id must be provided. */
    void add_state(int state, bool initial = false, bool accepting = false, int token_id = -1);
    /** Add a transition from src to dst on input symbol. */
    void add_transition(int src, char symbol, int dst);
    /** Add all transitions from src at once with all possible inputs. */
    void add_transitions(int src, std::unordered_map<char, int> dsts);
    /** Returns true if every state in the DFA has a transition for every input in the domain. */
    bool validate() const;

    /** Check if the DFA has a certain state ID */
    bool contains_state(int state) const;
    /** Transition function that returns the next states after getting a certain input while in a certain state. */
    int transition(int statee, char symbol) const;
    /** Returns the token id of a state if it is an accepting state and -1 otherwise. */
    int accept(int state) const;

    /** Returns the minimal equivalent DFA */
    // DFA minimize() const;
};

#endif