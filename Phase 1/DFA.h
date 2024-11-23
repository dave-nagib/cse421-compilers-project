#ifndef DFA_H
#define DFA_H
#include<unordered_map>
#include<unordered_set>
#include<bits/stdc++.h>
using namespace std;

class DFA {
  private:
    unordered_set<int> states;
    unordered_map<int, unordered_map<char, int>> transitions;
    int initial_state;
    unordered_map<int, int> accepting_states;
  
  public:
    /** Default constructor */
    DFA();
    /** DI constructor */
    DFA(unordered_set<int> states, unordered_map<int, unordered_map<char, int>> transitions, int initial_state, unordered_map<int, int> accepting_states);
    /** Add a state to the DFA. The state can be an initial and/or accepting. If it is accepting, a pattern_id must be provided. */
    void add_state(int state, bool initial = false, bool accepting = false, int token_id = -1);
    /** Check if the DFA has a certain state ID */
    bool contains_state(int state) const;
    /** Add a transition from src to dst on input symbol. */
    void add_transition(int src, char symbol, int dst);
    /** Add all transitions from src at once with all possible inputs. */
    void add_transitions(int src, unordered_map<char, int> dsts);
    /** Returns the initial state */
    int get_initial() const;
    /** Returns a map of accepting states and their corresponding token ids. */
    unordered_map<int, int> get_accepting() const;
    /** Returns the token id of a state if it is an accepting state and -1 otherwise. */
    int accept(int state) const;
    /** Returns the set of states of the DFA. */
    unordered_set<int> get_states() const;
    /** Transition function that returns the next states after getting a certain input while in a certain state. */
    int transition(int statee, char symbol) const;
    /** Returns true if every state in the DFA has a transition for every input in the given domain. */
    bool validate(vector<char> domain) const;
};

#endif