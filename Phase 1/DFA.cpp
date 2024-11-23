#include "DFA.h"


DFA::DFA() : states(), transitions(), initial_state(-1), accepting_states() {}


DFA::DFA(unordered_set<int> states, 
         unordered_map<int, unordered_map<char, int>> transitions, 
         int initial_state, 
         unordered_map<int, int> accepting_states) {
  this->states = move(states);
  this->transitions = move(transitions);
  this->add_state(initial_state, true);
  for (auto pair: accepting_states) this->add_state(pair.first, false, true, pair.second);
}


void DFA::add_state(int state, bool initial = false, bool accepting = false, int token_id = -1) {
  // Assert state is not negative
  if(state < 0) throw runtime_error("State ID is negative.");
  states.insert(state);
  if(initial) this->initial_state = state;
  if (accepting) {
    if (token_id == -1) throw runtime_error("Pattern ID not provided for an accepting state.");
    accepting_states[state] = token_id;
  }
}


void DFA::add_transition(int src, char symbol, int dst) {
  // Assert states are in the set
  if(!this->contains_state(src) || !this->contains_state(dst))
    throw runtime_error("Invalid source or destination states.");
  transitions[src][symbol] = dst;
}


void DFA::add_transitions(int src, unordered_map<char, int> dsts) {
  // Assert states are in the set
  if(!this->contains_state(src))
    throw runtime_error("Invalid source state.");
  for (auto pair : dsts) {
    if (!this->contains_state(pair.second)) throw runtime_error("Invalid destination state.");
  }
  transitions[src] = move(dsts);
}


int DFA::get_initial() const {
  return initial_state;
}


unordered_map<int, int> DFA::get_accepting() const {
  return accepting_states;
}


int DFA::accept(int state) const {
  auto res = accepting_states.find(state);
  return (res != accepting_states.end())? res->second : -1;
}


unordered_set<int> DFA::get_states() const {
  return states;
}


int DFA::transition(int state, char symbol) const {
  if (!this->contains_state(state)) throw runtime_error("Invalid source state.");
  auto res_src = transitions.find(state);
  if (res_src == transitions.end()) throw runtime_error("DFA is incomplete. A state has no transitions.");
  auto res_symbol = res_src->second.find(symbol);
  if (res_symbol == res_src->second.end()) throw runtime_error("Unknown input symbol.");
  return res_symbol->second;
}


bool DFA::contains_state(int state) const {
  return states.find(state) != states.end();
}


bool DFA::validate(vector<char> domain) const {
  for (int state : states) {
    auto state_trns = transitions.find(state);
    if (state_trns == transitions.end()) return false;
    for (char symbol : domain) {
      if (state_trns->second.find(symbol) == state_trns->second.end()) 
        return false;
    }
  }
  return true;
}