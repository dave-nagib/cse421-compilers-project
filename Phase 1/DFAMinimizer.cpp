#include "DFAMinimizer.h"
using namespace std;

DFAMinimizer::DFAMinimizer(DFA& dfa) : dfa(dfa) {
  if (!dfa.validate()) throw runtime_error("Invalid or incomplete DFA passed to DFAMinimizer.");
}

map<pair<int, int>, bool> DFAMinimizer::distinguish_states() const {
  map<pair<int,int>,bool> distinguishable;

  // Initial pass: mark accepting & non-accepting pairs
  int i_final, j_final;
  auto states = this->dfa.get_states();

  for (auto i = states.begin(); i != states.end(); ++i) {
    for (auto j = next(i); j != states.end(); ++j) {
      i_final = this->dfa.accept(*i);
      j_final = this->dfa.accept(*j);
      // Mark pair if one state is accepting and the other is not or both are accepting with different token ids
      // Mark the pair where first ID < second ID
      if (*i < *j) 
        distinguishable[{*i,*j}] = (i_final != j_final);
      else 
        distinguishable[{*j,*i}] = (i_final != j_final);
    }
  }

  // Keep marking pairs until no new pairs are marked
  bool changed = true;
  int dst_i, dst_j, i, j;
  while (changed) {
    changed = false;
    for (auto s_pair : distinguishable) {

      if (!s_pair.second) { // If the pair is marked, skip it
        for (char symbol : this->dfa.get_input_domain()) {
          i = s_pair.first.first; 
          j = s_pair.first.second;
          dst_i = this->dfa.transition(i, symbol); 
          dst_j = this->dfa.transition(j, symbol);
          if (dst_i > dst_j) swap(dst_i, dst_j); // Make sure the first destination ID is smaller
          if (dst_i != dst_j && distinguishable[{dst_i, dst_j}]) {
            distinguishable[{i,j}] = true;
            changed = true;
          }
        }
      }
      
    }
  }
  return distinguishable;
}


DFA DFAMinimizer::partition_dfa(unordered_map<int, int>& partition) const {
  // Initialize new states and transitions
  unordered_set<int> new_states;
  unordered_map<int, unordered_map<char, int>> new_transitions;
  unordered_map<int, int> new_accepting_states;
  // Loop over each state mapping s = (S, S')
  for (auto &s : partition) {
    // Add the state mapping to new state if it wasn't added before
    new_states.insert(s.second);
    // Add the mapped transitions out of the original state
    for (auto &tr : this->dfa.get_transitions_from(s.first)){ // all transitions (a,T) from S
      new_transitions[s.second][tr.first] = partition[tr.second]; // S' -> T' on input a
    }
    // If the original state accepts some token, make the mapped state accept the same token
    int token = this->dfa.accept(s.first);
    if (token != -1)
      new_accepting_states[s.second] = token;
  }
  // Construct and return the new DFA
  return DFA(
    this->dfa.get_input_domain(),
    move(new_states),
    move(new_transitions),
    partition[this->dfa.get_initial()],
    new_accepting_states
  );
}


void DFAMinimizer::remove_unreachable() const {
  // Initialize the set of unreachable states with all states minus the initial state
  unordered_set<int> unreachable_states = this->dfa.get_states();
  // Initialize the queue of states to visit in BFS traversal
  queue<int> to_visit;
  to_visit.push(this->dfa.get_initial());
  while (!to_visit.empty()) {
    int current = to_visit.front();
    to_visit.pop();
    unreachable_states.erase(current);
    for (auto& tr : this->dfa.get_transitions_from(current)) {
      if (unreachable_states.find(tr.second) != unreachable_states.end())
        to_visit.push(tr.second);
    }
  }
  // Remove the unreachable states, their transitions, and accepting state tokens from the DFA
  for (int s : unreachable_states) this->dfa.remove_state(s, false);
}


DFA DFAMinimizer::minimize() const {
  // Remove unreachable states
  this->remove_unreachable();
  // Construct the partition mapping (state -> partition number) based on distinguishable states
  unordered_map<int, int> partition;
  int mapping_counter = 1;
  { // Code block to limit the scope of distinguishable
    map<pair<int,int>,bool> distinguishable = this->distinguish_states();
    // Collect indistinguishable pairs
    int i, j;
    for (auto& s_pair : distinguishable) {
      if (!s_pair.second) {
        i = s_pair.first.first; j = s_pair.first.second;
        // See if one of the states is already mapped and map it to the same value
        if (partition.find(i) != partition.end())
          partition[j] = partition[i];
        else if (partition.find(j) != partition.end())
          partition[i] = partition[j];
        else
          partition[i] = partition[j] = mapping_counter++; // Otherwise, map both to a new value
      }
    }
  } // End of code block

  // Map the remaining unmapped states to a new value each
  for (int s : this->dfa.get_states()) {
    if (partition[s] == 0) partition[s] = mapping_counter++;
  }

  // Construct and return the new DFA based on the partition
  return this->partition_dfa(partition);
}