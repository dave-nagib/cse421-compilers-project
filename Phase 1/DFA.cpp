#include "DFA.h"
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
  this->add_state(initial_state, true);
  for (auto pair: accepting_states) this->add_state(pair.first, false, true, pair.second);
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


bool DFA::validate() const {
  for (int state : states) {
    auto state_trns = transitions.find(state);
    if (state_trns == transitions.end()) return false;
    for (char symbol : input_domain) {
      if (state_trns->second.find(symbol) == state_trns->second.end()) 
        return false;
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
  if (res_src == transitions.end()) throw runtime_error("DFA is incomplete. A state has no transitions.");
  auto res_symbol = res_src->second.find(symbol);
  if (res_symbol == res_src->second.end()) throw runtime_error("Unknown input symbol.");
  return res_symbol->second;
}


int DFA::accept(int state) const {
  auto res = accepting_states.find(state);
  return (res != accepting_states.end())? res->second : -1;
}


// DFA DFA::minimize() const {

//   unordered_map<int, int> partition;
//   { // Code block to limit the scope of distinguishable
//     map<pair<int,int>,bool> distinguishable = this->distinguish_states();
//     // Collect indistinguishable pairs
//     int counter = 1, i, j;
//     for (auto& s_pair : distinguishable) {
//       if (!s_pair.second) {
//         i = s_pair.first.first; j = s_pair.first.second;
//         // See if one of the states is already mapped and map it to the same value
//         if (partition.find(i) != partition.end())
//           partition[j] = partition[i];
//         else if (partition.find(j) != partition.end())
//           partition[i] = partition[j];
//         else
//           partition[i] = partition[j] = counter++; // Otherwise, map both to a new value
//       }
//     }
//     // Map the remaining states to new values
//     for (int s : this->states) {
//       if (partition.find(s) == partition.end()) partition[s] = counter++;
//     }
//   } // End of code block

//   // Construct and return the new DFA based on the partition
//   return this->partition_dfa(partition);
// }


// map<pair<int,int>,bool> DFA::distinguish_states() const {
//   map<pair<int,int>,bool> distinguishable;
//   // Initial pass: mark accepting & non accepting pairs
//   int i_final, j_final;
//   for (auto i = this->states.begin(); i != states.end(); i++) {
//     for (auto j = next(i); j != this->states.end(); j++) {
//       i_final = this->accept(*i);
//       j_final = this->accept(*j);
//       // Mark pair if one state is accepting and the other is not or both are accepting with different token ids
//       // Mark the pair where first ID < second ID
//       if (*i > *j) distinguishable[{*i,*j}] = (i_final != j_final);
//       else distinguishable[{*j,*i}] = (i_final != j_final);
//     }
//   }
//   // Keep marking pairs until no new pairs are marked
//   bool changed = true;
//   int dst_i, dst_j, i, j;
//   while (changed) {
//     changed = false;
//     for (auto s_pair : distinguishable) {

//       if (!s_pair.second) { // If the pair is marked, skip it
//         for (char symbol : input_domain) {
//           i = s_pair.first.first; j = s_pair.first.second;
//           dst_i = this->transition(i, symbol); dst_j = this->transition(j, symbol);
//           if (dst_i > dst_j) swap(dst_i, dst_j); // Make sure the first destination ID is smaller
//           if (dst_i != dst_j && distinguishable[{dst_i, dst_j}]) {
//             distinguishable[{i,j}] = true;
//             changed = true;
//           }
//         }
//       }
      
//     }
//   }
//   return distinguishable;
// }


// DFA DFA::partition_dfa(unordered_map<int, int>& partition) const {
//   // Initialize new states and transitions
//   unordered_set<int> new_states;
//   unordered_map<int, unordered_map<char, int>> new_transitions;
//   unordered_map<int, int> new_accepting_states;
//   // Loop over each state mapping s = (S, S')
//   for (auto &s : partition) {
//     // Add the state mapping to new state if it wasn't added before
//     new_states.insert(s.second);
//     // Add the mapped transitions out of the original state
//     for (auto &tr : this->transitions.at(s.first)){ // all transitions (a,T) from S
//       new_transitions[s.second][tr.first] = partition[tr.second]; // S' -> T' on input a
//     }
//     // If the original state accepts some token, make the mapped state accept the same token
//     int token = this->accept(s.first);
//     if (token != -1)
//       new_accepting_states[s.second] = token;
//   }
//   // Construct and return the new DFA
//   return DFA(
//     this->input_domain,
//     move(new_states),
//     move(new_transitions),
//     partition[this->initial_state],
//     new_accepting_states
//   );
// }
