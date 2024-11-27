#ifndef NFA_H
#define NFA_H
#include<unordered_map>
#include<unordered_set>
#include<bits/stdc++.h>

class NFA {
  private:
    std::unordered_set<int> states;
    std::unordered_map<int, std::unordered_map<char, std::vector<int>>> transitions;
    int initial_state;
    std::unordered_map<int, int> accepting_states;
  
  public:
    /** Default constructor */
    NFA();
    /** DI constructor */
    NFA(
      std::unordered_set<int> states, 
      std::unordered_map<int, std::unordered_map<char, std::vector<int>>> transitions, 
      int initial_state, 
      std::unordered_map<int, int> accepting_states
    );

    /** Returns the epsilon closure (all reachable states by empty transitions) of a set of states. */
    std::unordered_set<int> eps_closure(std::unordered_set<int> states) const;
    /** Utility function to copy an NFA with new state IDs by incrementing old ones */
    NFA copy_with_new_ids(int offset) const;

    /** Returns the epsilon closure of the initial state */
    std::unordered_set<int> get_initial() const;
    /** Returns the set of states of the NFA. */
    std::unordered_set<int> get_states() const;
    /** Returns a map of accepting states and their corresponding token ids. */
    std::unordered_map<int, int> get_accepting() const;
    /** Returns the accepting state of the NFA. Assumes that the NFA has only one accepting state. */
    int get_single_accepting() const;

    /** Add a state to the NFA. */
    void add_state(int state);
    /** Set a state to be the initial state. */
    void make_initial(int state);
    /** Set a state to be an accepting state with a certain token id. */
    void make_accepting(int state, int token_id);
    /** Add a transition from src to dst on input symbol. */
    void add_transition(int src, char symbol, int dst);

    /** Check if the NFA has a certain state ID */
    bool contains_state(int state) const;
    /** Transition function that returns the epsilon closure of next states after getting a certain input while in a certain set of states. */
    std::unordered_set<int> transition(std::unordered_set<int> states, char symbol) const;
    /** Returns the token id of a state if it is an accepting state and -1 otherwise. */
    int accept(int state) const;
    /** 
     * Returns the minimum token id of an accepting state among a set of states if there is one or more. 
     * If there are no accepting states, returns 1. 
     */
    int accept(std::unordered_set<int> states) const;

    /**
     * Returns a new NFA that accepts the union of the languages of a list of NFAs. Corresponds to the '|' operator in regex.
     * Can have a common accpeted lagnuage or differentiate between the accepted language of each input NFA.  
     * **Assumes that all input NFAs have a single accepting state.**
     */
    static NFA union_nfa(std::vector<NFA>& nfas, int new_start, bool common_accept = false, int new_accepting = -1, int token_id = -1);
    /**
     * Returns a new NFA that accepts the union of the languages of this NFA and another NFA. Corresponds to the '|' operator in regex.    
     * **Assumes that both NFAs have a single accepting state.**
     */
    NFA simple_union(NFA& other, int new_start, int new_accept, int token_id) const;
    /**
     * Returns a new NFA that accepts the concatenation of the language of this NFA followed by another NFA's language. Corresponds to concatenation in regex.  
     * **Assumes that both NFAs have a single accepting state.**
     */
    NFA simple_concat(NFA& other, int token_id) const;
    /**
     * Returns a new NFA that accepts the Kleene or Positive closures of the language of this NFA. Corresponds to the '*' or '+' operators in regex.  
     * **Assumes that this NFA has a single accepting state.**
     */
    NFA simple_repeat(bool zero_or_more, int new_accept, int token_id) const;

    /** Function to print the NFA components for debugging */
    void print_nfa() const;
};

#endif