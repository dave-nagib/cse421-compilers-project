#include "DFA.h"
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunqualified-std-cast-call"
using namespace std;

DFA::DFA(vector<char> input_domain) : input_domain(move(input_domain)), transitions(), initial_state(-1), accepting_states() {}


DFA::DFA(vector<char> input_domain,
         unordered_set<int> states, 
         unordered_map<int, unordered_map<char, int>> transitions, 
         int initial_state, 
         unordered_map<int, int> accepting_states) {
  this->input_domain = move(input_domain);
  this->states = move(states);
  this->transitions = move(transitions);
  this->make_initial(initial_state);
  for (auto pair: accepting_states) this->make_accepting(pair.first, pair.second);
}


vector<char> DFA::get_input_domain() const {
  return input_domain;
}


int DFA::get_initial() const {
  return initial_state;
}


unordered_set<int> DFA::get_states() const {
  return states;
}


unordered_map<int, int> DFA::get_accepting() const {
  return accepting_states;
}


unordered_map<char, int> DFA::get_transitions_from(int state) const {
  return transitions.at(state);
}


void DFA::add_state(int state) {
  // Assert state is not negative
  if(state < 0) throw runtime_error("State ID is negative.");
  states.insert(state);
}


void DFA::remove_state(int state, bool reachable) {
  if (!this->contains_state(state)) throw runtime_error("State to be removed does not exist in the DFA.");
  if (initial_state == state) throw runtime_error("Cannot remove the initial state.");
  // Erase state and its outgoing transitions
  states.erase(state);
  transitions.erase(state);
  accepting_states.erase(state);
  // If the state is reachable, erase all ingoing transitions
  if (reachable) {
    // Loop over state - outgoing transitions
    for (auto& pair : transitions) {
        // Loop over input - destination for a fixed source state
        for (auto& ip_dst : pair.second) {
            // If destination is the state, erase the whole transition
            if (ip_dst.second == state) pair.second.erase(ip_dst.first);
        }
    }
  }
}


void DFA::make_initial(int state) {
  if (!this->contains_state(state)) throw runtime_error("State to made initial does not exist in the DFA.");  
  this->initial_state = state;
}


void DFA::make_accepting(int state, int token_id) {
  if (!this->contains_state(state)) throw runtime_error("State to be made accepting does not exist in the DFA.");
  accepting_states[state] = token_id;
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


bool DFA::validate() const {
  for (int state : states) {
    auto state_trns = transitions.find(state);
    if (state_trns == transitions.end()) return false;
    for (char symbol : input_domain) {
      if (state_trns->second.find(symbol) == state_trns->second.end()) {
          cout << "Missing symbol from DFA: " << symbol << endl;
          return false;
      }
    }
  }
  return true;
}


bool DFA::contains_state(int state) const {
  return states.find(state) != states.end();
}


int DFA::transition(int state, char symbol) const {
  if (!this->contains_state(state)) throw runtime_error("Invalid source state.");
  auto res_src = transitions.find(state);
  if (res_src == transitions.end()) throw runtime_error("State has no outgoing transitions. DFA is incomplete.");
  auto res_symbol = res_src->second.find(symbol);
  if (res_symbol == res_src->second.end()) throw runtime_error("Unknown input symbol.");
  return res_symbol->second;
}


int DFA::accept(int state) const {
  auto res = accepting_states.find(state);
  return (res != accepting_states.end())? res->second : -1;
}


void DFA::print_dfa(unordered_map<char, char> tokenChars, unordered_map<int, string> tokens) const {
  cout << "DFA components:\n" << endl;

  cout << "Input domain: ";
  for (char symbol : input_domain) cout << symbol << " ";
  cout << endl;

  cout << "States: ";
  for (int state : states) cout << state << " ";
  cout << endl;

  cout << "Initial state: " << initial_state << endl;

  cout << "Accepting states:\n";
  for (auto pair : accepting_states) cout << "\t" << pair.first << " with token id " << pair.second << " token " <<tokens.at(accepting_states.at(pair.first)) << endl;

  cout << "Transitions:" << endl;
  for (const auto& pair : transitions) {
    if (accepting_states.find(pair.first) != accepting_states.end()){
        cout << "\tFrom state " << pair.first  << " Accepting class with token number " << tokens.at(accepting_states.at(pair.first)) << ":" << endl;
    }else
      cout << "\tFrom state " << pair.first << ":" << endl;
    for (auto tr : pair.second) {
      if (accepting_states.find(tr.second) != accepting_states.end()){

          cout << "\t\t---- " << tokenChars.at(tr.first) << " ----> " << tr.second << " token: " << tokens.at(accepting_states.at(tr.second)) << endl;
          continue;
      }
        cout << "\t\t---- " << tokenChars.at(tr.first) << " ----> " << tr.second << endl;
    }
  }
}

void DFA::print_dfa(unordered_map<char, char> tokenChars, unordered_map<int, string> tokens, const string& output_file_path) const {
    ofstream output_file(output_file_path);
    if (!output_file.is_open()) {
        cerr << "Error: Could not open file "
             << output_file_path
             << " for writing." << endl;
        return;
    }

    output_file << "DFA components:\n" << endl;

    output_file << "Input domain: ";
    for (char symbol: input_domain) output_file << tokenChars.at(symbol) << " ";
    output_file << endl;

    output_file << "States: ";
    for (int state: states) output_file << state << " ";
    output_file << endl;

    output_file << "Initial state: " << initial_state << endl;

    output_file << "Accepting states:\n";
    for (auto pair: accepting_states)
        output_file << "\t" << pair.first << " with token id " << pair.second << " token "
                    << tokens.at(accepting_states.at(pair.first)) << endl;

    output_file << "Transitions:" << endl;
    for (const auto &pair: transitions) {
        if (accepting_states.find(pair.first) != accepting_states.end()) {
            output_file << "\tFrom state " << pair.first << " Accepting class with token number "
                        << tokens.at(accepting_states.at(pair.first)) << ":" << endl;
        } else
            output_file << "\tFrom state " << pair.first << ":" << endl;
        for (auto tr: pair.second) {
            if (accepting_states.find(tr.second) != accepting_states.end()) {

                output_file << "\t\t---- " << tokenChars.at(tr.first) << " ----> " << tr.second << " token: "
                            << tokens.at(accepting_states.at(tr.second)) << endl;
                continue;
            }
            output_file << "\t\t---- " << tokenChars.at(tr.first) << " ----> " << tr.second << endl;
        }
    }
}

int DFA::get_dead_state() const {
    int dead_state = -1;
    for (int state : states) {
        // Skip accepting states
        if (accepting_states.find(state) != accepting_states.end()) {
            continue;
        }

        // Check if the state is dead
        bool is_dead = true;
        for (char symbol : input_domain) {
            if (transitions.at(state).find(symbol) != transitions.at(state).end()
                && transitions.at(state).at(symbol) != state) {
                is_dead = false;
                break;
            }
        }
        if (is_dead) dead_state = state;
    }
    // there is no dead state if dead_state is -1
    return dead_state;
}




#pragma clang diagnostic pop