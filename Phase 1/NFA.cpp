#include "NFA.h"
using namespace std;

NFA::NFA() : states(), transitions(), initial_state(-1), accepting_states() {}


NFA::NFA(unordered_set<int> states, 
          unordered_map<int, unordered_map<char, vector<int>>> transitions, 
          int initial_state, 
          unordered_map<int, int> accepting_states) {
  this->states = move(states);
  this->transitions = move(transitions);
  this->add_state(initial_state, true);
  for (auto pair: accepting_states) this->add_state(pair.first, false, true, pair.second);
}


void NFA::add_state(int state, bool initial = false, bool accepting = false, int token_id = -1) {
  // Assert state is not negative
  if(state < 0) throw runtime_error("State ID is negative.");
  states.insert(state);
  if(initial) this->initial_state = state;
  if (accepting) {
    if (token_id == -1) throw runtime_error("Pattern ID not provided for an accepting state.");
    accepting_states[state] = token_id;
  }
}


bool NFA::contains_state(int state) const {
  return states.find(state) != states.end();
}


void NFA::add_transition(int src, char symbol, int dst) {
  // Assert states are in the set
  if(!this->contains_state(src) || !this->contains_state(dst))
    throw runtime_error("Invalid source or destination states.");
  transitions[src][symbol].emplace_back(dst);
}


unordered_set<int> NFA::eps_closure(unordered_set<int> states) const {
  for (int state : states) {
    if (!this->contains_state(state)) throw runtime_error("Invalid state in epsilon closure.");
  }
  unordered_set<int> closure = states; // Initialize closure with the input states
  // Use a stack to keep track of states that need to be visited
  stack<int> stack;
  for (int state : states) stack.push(state);
  // Iteration variables
  int state;
  unordered_map<int, unordered_map<char, vector<int>>>::const_iterator state_trns;
  unordered_map<char, vector<int>>::const_iterator eps_trns;
  // Keep adding states reachable by epsilon transitions to the closure until stack is empty
  while (!stack.empty()) {
    // Get the next state from the stack and skip it if it has no epsilon transitions
    state = stack.top(); stack.pop();
    if ((state_trns = transitions.find(state)) == transitions.end()) continue;
    if ((eps_trns = state_trns->second.find('\0')) == state_trns->second.end()) continue;
    // Add all states reachable by epsilon transitions from current state to the closure
    for (int dst : eps_trns->second) {
      if (closure.find(dst) == closure.end()) {
        closure.insert(dst);
        stack.push(dst);
      }
    }
  }
  return closure;
}


unordered_set<int> NFA::get_initial() const {
  if (initial_state == -1) throw runtime_error("NFA doesn't have an initial state yet.");
  return eps_closure({initial_state});
}


unordered_map<int, int> NFA::get_accepting() const {
  return accepting_states;
}


int NFA::get_single_accepting() const {
  if (accepting_states.size() != 1) throw runtime_error("NFA has zero or more than one accepting states.");
  return accepting_states.begin()->first;
}


int NFA::accept(int state) const {
  auto res = accepting_states.find(state);
  return (res != accepting_states.end())? res->second : -1;
}


unordered_set<int> NFA::get_states() const {
  return states;
}


unordered_set<int> NFA::transition(unordered_set<int> states, char symbol) const{
  for (int state : states) {
    if (!this->contains_state(state)) throw runtime_error("Invalid state in transition.");
  }
  unordered_set<int> next_states;
  unordered_map<int, unordered_map<char, vector<int>>>::const_iterator res1;
  unordered_map<char, vector<int>>::const_iterator res2;
  for (int state : states) {
    if ((res1 = transitions.find(state)) == transitions.end()) continue; // No transitions from this state
    if ((res2 = res1->second.find(symbol)) == res1->second.end()) continue; // No transitions on this symbol
    for (int dst : res2->second) next_states.insert(dst);
  }
  return this->eps_closure(next_states);
}


NFA NFA::union_nfa(vector<NFA>& nfas, int new_start, bool common_accept = false, int new_accepting = -1, int token_id = -1) {

  // Combine the states of all NFAs into a new set
  unordered_set<int> new_states;
  int expected_size = 0;
  for (const NFA& nfa : nfas) {
    new_states.insert(nfa.states.begin(), nfa.states.end());
    expected_size += nfa.states.size();
  }
  if (new_states.find(new_start) != new_states.end())
    throw runtime_error("New start state already exists in one of the NFAs.");
  if (new_states.size() != expected_size)
    throw runtime_error("There is overlap between the states of the NFAs.");
  // Combine the transitions of all NFAs into a new map
  unordered_map<int, unordered_map<char, vector<int>>> new_transitions;
  for (const NFA& nfa : nfas) 
    new_transitions.insert(nfa.transitions.begin(), nfa.transitions.end());
  // Construct the new NFA with combined states and transitions
  NFA combined_nfa = NFA(new_states, new_transitions, new_start, {});
  for (const NFA& nfa : nfas) combined_nfa.add_transition(new_start, '\0', nfa.initial_state);
  // Choose a common accepting state or use multiple accepting states
  if (common_accept) {
    if (new_accepting == -1) throw runtime_error("New accept state ID not provided.");
    if (new_states.find(new_start) != new_states.end())
      throw runtime_error("New start state already exists in one of the NFAs.");
    if (token_id == -1) throw runtime_error("Pattern ID not provided.");
    combined_nfa.add_state(new_accepting, false, true, token_id);
    for (const NFA& nfa : nfas) combined_nfa.add_transition(nfa.get_single_accepting(), '\0', new_accepting);
  } else {
    for (const NFA& nfa : nfas) combined_nfa.add_state(nfa.get_single_accepting(), false, true, nfa.accept(nfa.get_single_accepting()));
  }
}


NFA NFA::simple_union(NFA& other, int new_start, int new_accept, int token_id) const {
  // Combine the states of both NFAs into a new set
  auto new_states = this->states;
  new_states.insert(other.states.begin(), other.states.end());
  if (new_states.find(new_start) != new_states.end() || new_states.find(new_accept) != new_states.end())
    throw runtime_error("New start or accept state already exists in one of the NFAs.");
  if (new_states.size() != this->states.size() + other.states.size())
    throw runtime_error("There is overlap between the states of the two NFAs.");
  // Combine the transitions of both NFAs into a new map
  auto new_transitions = this->transitions;
  new_transitions.insert(other.transitions.begin(), other.transitions.end());
  // Get the starting and ending states of both NFAs
  int self_start = this->initial_state, self_accept = this->get_single_accepting();
  int other_start = other.initial_state, other_accept = other.get_single_accepting();
  // Construct the new NFA with combined states and transitions
  NFA nfa = NFA(new_states, new_transitions, new_start, {{new_accept, token_id}}); //TODO: check syntax
  // Add transitions from the new start state to the start states of both NFAs
  nfa.add_transition(new_start, '\0', self_start);
  nfa.add_transition(new_start, '\0', other_start);
  // Add transitions from the accept states of both NFAs to the new accept state
  nfa.add_transition(self_accept, '\0', new_accept);
  nfa.add_transition(other_accept, '\0', new_accept);
  return nfa;
}


NFA NFA::simple_concat(NFA& other, int token_id) const {
  // Combine the states of both NFAs into a new set
  auto new_states = this->states;
  new_states.insert(other.states.begin(), other.states.end());
  if (new_states.size() != this->states.size() + other.states.size())
    throw runtime_error("There is overlap between the states of the two NFAs.");
  // Combine the transitions of both NFAs into a new map
  auto new_transitions = this->transitions;
  new_transitions.insert(other.transitions.begin(), other.transitions.end());
  // Get the starting and ending states of both NFAs
  int self_start = this->initial_state, self_accept = this->get_single_accepting();
  int other_start = other.initial_state, other_accept = other.get_single_accepting();
  // Construct the new NFA with combined states and transitions
  NFA nfa = NFA(new_states, new_transitions, self_start, {{other_accept, token_id}});
  // Add an empty transition from this NFA's accepting state to the other NFA's start
  nfa.add_transition(self_accept, '\0', other_start);
  return nfa;
}


NFA NFA::simple_repeat(bool zero_or_more, int new_accept, int token_id) const {
  // Get start and ending states of this NFA
  int self_start = this->initial_state, old_accept = this->get_single_accepting();
  // Construct a new NFA with the same states and transitions and a new accept state
  NFA nfa = NFA(this->states, this->transitions, this->initial_state, {{new_accept, token_id}});
  // Add an empty transition from old end to start (loop)
  nfa.add_transition(old_accept, '\0', self_start);
  // Add an empty transition from old end to new end
  nfa.add_transition(old_accept, '\0', new_accept);
  // Add an empty transition from start to new end if it's zero or more
  if (zero_or_more) nfa.add_transition(self_start, '\0', new_accept);
  return nfa;
}