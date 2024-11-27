#include "NFA.h"
using namespace std;

NFA::NFA() : states(), transitions(), initial_state(-1), accepting_states() {}


NFA::NFA(unordered_set<int> states, 
          unordered_map<int, unordered_map<char, vector<int>>> transitions, 
          int initial_state, 
          unordered_map<int, int> accepting_states) {
  this->states = move(states);
  this->transitions = move(transitions);
  this->make_initial(initial_state);
  for (auto pair: accepting_states) this->make_accepting(pair.first, pair.second);
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


NFA NFA::copy_with_new_ids(int offset) const {
  // Copy the states and transitions with new state IDs
  unordered_set<int> new_states;
  unordered_map<int, unordered_map<char, vector<int>>> new_transitions;
  unordered_map<int, int> new_accepting_states;
  for (int state : states) {
    new_states.insert(state + offset);
    if (transitions.find(state) != transitions.end()) {
      for (auto tr : transitions.at(state)) {
        for (int dst : tr.second) new_transitions[state + offset][tr.first].push_back(dst + offset);
      }
    }
  }
  for (auto pair : accepting_states) new_accepting_states[pair.first + offset] = pair.second;
  return NFA(new_states, new_transitions, initial_state + offset, new_accepting_states);
}


unordered_set<int> NFA::get_initial() const {
  if (initial_state == -1) throw runtime_error("NFA doesn't have an initial state yet.");
  return eps_closure({initial_state});
}


unordered_set<int> NFA::get_states() const {
  return states;
}


unordered_map<int, int> NFA::get_accepting() const {
  return accepting_states;
}


int NFA::get_single_accepting() const {
  if (accepting_states.size() != 1) throw runtime_error("NFA has zero or more than one accepting states.");
  return accepting_states.begin()->first;
}


void NFA::add_state(int state) {
  // Assert state is not negative
  if(state < 0) throw runtime_error("State ID is negative.");
  states.insert(state);
}


void NFA::make_initial(int state) {
  if (!this->contains_state(state)) throw runtime_error("State to made initial does not exist in the NFA.");  
  this->initial_state = state;
}


void NFA::make_accepting(int state, int token_id) {
  if (!this->contains_state(state)) throw runtime_error("State to be made accepting does not exist in the NFA.");
  this->accepting_states[state] = token_id;
}


void NFA::add_transition(int src, char symbol, int dst) {
  // Assert states are in the set
  if(!this->contains_state(src) || !this->contains_state(dst))
    throw runtime_error("Invalid source or destination states.");
  transitions[src][symbol].emplace_back(dst);
}


bool NFA::contains_state(int state) const {
  return states.find(state) != states.end();
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


int NFA::accept(int state) const {
  auto res = accepting_states.find(state);
  return (res != accepting_states.end())? res->second : -1;
}


int NFA::accept(unordered_set<int> states) const {
  int min_token = INT_MAX;
  for (int state : states) {
    int token = this->accept(state);
    if (token != -1) min_token = min(min_token, token);
  }
  return min_token;
}


NFA NFA::union_nfa(vector<NFA>& nfas, int new_start, bool common_accept, int new_accepting, int token_id) {

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
  // Add the new start state to the set of states
  new_states.insert(new_start);
  // Combine the transitions of all NFAs into a new map
  unordered_map<int, unordered_map<char, vector<int>>> new_transitions;
  for (const NFA& nfa : nfas) 
    new_transitions.insert(nfa.transitions.begin(), nfa.transitions.end());
  // Get all initial states adding them to a vector, and add transitions from the new start state to them
  vector<int> initial_states;
  for (const NFA& nfa : nfas) initial_states.push_back(nfa.initial_state);
  new_transitions[new_start]['\0'] = move(initial_states); // Epsilon transition from new start to all initial states
  // Construct the new NFA with combined states and transitions
  NFA combined_nfa = NFA(new_states, new_transitions, new_start, {});
  // Choose a common accepting state or use multiple accepting states
  if (common_accept) {
    if (new_accepting == -1) throw runtime_error("New accept state ID not provided.");
    if (new_states.find(new_accepting) != new_states.end())
      throw runtime_error("New common accepting state already exists in one of the NFAs.");
    if (token_id == -1) throw runtime_error("Pattern ID not provided.");
    combined_nfa.add_state(new_accepting);
    combined_nfa.make_accepting(new_accepting, token_id);
    for (const NFA& nfa : nfas) combined_nfa.add_transition(nfa.get_single_accepting(), '\0', new_accepting);
  } else {
    for (const NFA& nfa : nfas) {
      int nfa_accepting = nfa.get_single_accepting();
      combined_nfa.add_state(nfa_accepting);
      combined_nfa.make_accepting(nfa_accepting, nfa.accept(nfa_accepting));
    }
  }
  return combined_nfa;
}


NFA NFA::simple_union(NFA& other, int new_start, int new_accept, int token_id) const {
  // Combine the states of both NFAs into a new set
  auto new_states = this->states;
  new_states.insert(other.states.begin(), other.states.end());
  if (new_states.find(new_start) != new_states.end() || new_states.find(new_accept) != new_states.end())
    throw runtime_error("New start or accept state already exists in one of the NFAs.");
  if (new_states.size() != this->states.size() + other.states.size())
    throw runtime_error("There is overlap between the states of the two NFAs.");
  // Insert the new start and accept states into the set of states
  new_states.insert(new_start);
  new_states.insert(new_accept);
  // Combine the transitions of both NFAs into a new map
  auto new_transitions = this->transitions;
  new_transitions.insert(other.transitions.begin(), other.transitions.end());
  // Add starting and ending epsilon transitions
  new_transitions[new_start]['\0'] = {this->initial_state, other.initial_state};
  new_transitions[this->get_single_accepting()]['\0'].push_back(new_accept);
  new_transitions[other.get_single_accepting()]['\0'].push_back(new_accept);
  // Construct the new NFA with combined states and transitions
  return NFA(new_states, new_transitions, new_start, {{new_accept, token_id}});
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
  // Add epsilon transition from this NFA's accepting state to the other NFA's start
  new_transitions[this->get_single_accepting()]['\0'].push_back(other.initial_state);
  // Construct the new NFA with combined states and transitions
  return NFA(new_states, new_transitions, this->initial_state, {{other.get_single_accepting(), token_id}});
}


NFA NFA::simple_repeat(bool zero_or_more, int new_accept, int token_id) const {
  // Get start and ending states of this NFA
  int self_start = this->initial_state, old_accept = this->get_single_accepting();
  // Copy the states of this NFA
  auto new_states = this->states;
  new_states.insert(new_accept); // Add the new accept state
  // Copy the transitions of this NFA
  auto new_transitions = this->transitions;
  // Add an empty transition from old end to start (loop) and new end
  new_transitions[old_accept]['\0'].push_back(self_start);
  new_transitions[old_accept]['\0'].push_back(new_accept);
  // Add an empty transition from start to new end if it's zero or more
  if (zero_or_more) new_transitions[self_start]['\0'].push_back(new_accept);
  // Construct a new NFA with the same states and transitions and a new accept state
  return NFA(new_states, new_transitions, this->initial_state, {{new_accept, token_id}});
}


void NFA::print_nfa() const {
  cout << "NFA components:\n" << endl;

  cout << "States: ";
  for (int state : states) cout << state << " ";
  cout << endl;

  cout << "Initial state (epsilon closure): ";
  for (int state : this->get_initial()) cout << state << " ";
  cout << endl;

  cout << "Accepting states:\n";
  for (auto pair : accepting_states) cout << "\t" << pair.first << " with token " << pair.second << endl;

  cout << "Transitions:" << endl;
  for (auto pair : transitions) {
    cout << "\tFrom state " << pair.first << ":" << endl;
    for (auto tr : pair.second) {
      string ip = (tr.first == '\0')? "eps" : string(1, tr.first);
      cout << "\t\t---- " << ip << " ----> { ";
      for (int state : tr.second) cout << state << " ";
      cout << "}" << endl;
    }
  }
}