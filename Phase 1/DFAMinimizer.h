#ifndef DFA_MINIMIZER_H
#define DFA_MINIMIZER_H
#include "DFA.h"
#include <map>

class DFAMinimizer {
  private:
    DFA& dfa;
    /** Returns a map of all unique state pairs to a boolean indicating distinguishability */
    std::map<std::pair<int, int>, bool> distinguish_states() const; 
    /** Rebuilds the current DFA based on some input partition / mapping of old state ID -> new state ID */
    DFA partition_dfa(std::unordered_map<int, int>& partition) const; 
    /** Removes unreachable states from the DFA */
    void remove_unreachable() const;
  public:
    /** Constructor that has a read-only reference to a DFA */
    DFAMinimizer(DFA& dfa);
    /** Minimize a DFA */
    DFA minimize() const;
};

#endif