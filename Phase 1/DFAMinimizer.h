#ifndef DFA_MINIMIZER_H
#define DFA_MINIMIZER_H
#include "DFA.h"

class DFAMinimizer {
  private:
    const DFA& dfa;
    /** Returns a map of all unique state pairs to a boolean indicating distinguishability */
    std::map<std::pair<int, int>, bool> distinguish_states() const; 
    /** Rebuilds the current DFA based on some input partition / mapping of old state ID -> new state ID */
    DFA partition_dfa(std::unordered_map<int, int>& partition) const; 
  public:
    /** Constructor that has a read-only reference to a DFA */
    DFAMinimizer(const DFA& dfa) : dfa(dfa) {}
    /** Minimize a DFA using the Hopcroft's algorithm */
    DFA minimize() const;
};

#endif