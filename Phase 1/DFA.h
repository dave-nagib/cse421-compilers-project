#ifndef DFA_H
#define DFA_H
#include<unordered_map>
#include<unordered_set>
#include<vector>
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
    DFA() = default;
    /** Constructor that initializes the DFA with an input domain */
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
    /** Returns the dead state of the DFA */
    int get_dead_state() const;
    
    /** Add a state to the DFA. */
    void add_state(int state);
    /** 
     * Removes a non-initial state, its ingoing (if reachable) and outgoing transitions, 
     * and deletes it from accepting states if it was accepting. 
     */
    void remove_state(int state, bool reachable);
    /** Set a state to be the initial state. */
    void make_initial(int state);
    /** Set a state to be an accepting state with a certain token id. */
    void make_accepting(int state, int token_id);
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

    /** Function to print the DFA components for debugging */
    void print_dfa(std::unordered_map<char, char>, std::unordered_map<int, std::string>) const;
    /** Function to write the DFA components in a file */
    void print_dfa(std::unordered_map<char, char>, std::unordered_map<int, std::string>, const std::string&) const;
};

#endif