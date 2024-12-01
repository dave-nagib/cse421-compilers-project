#ifndef CSE421_COMPILERS_PROJECT_NFA2DFA_H
#define CSE421_COMPILERS_PROJECT_NFA2DFA_H

#include "NFA.h"
#include "DFA.h"
#include<bits/stdc++.h>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <vector>

class NFA2DFA {
    private:
        // Helper functions
        int NFA2DFA_map_state(const std::unordered_set<int>& target, const std::unordered_map<int, std::unordered_set<int>>& source);
        std::vector<char> NFA_get_input_domain(const NFA& nfa);

    public:
        NFA2DFA();
        /**
         * Converts an NFA into a DFA.
         *
         * @param nfa The NFA to be converted.
         * @param input_domain The input symbols that the DFA recognizes.
         * @return A DFA equivalent to the given NFA.
         */
        DFA convert(const NFA& nfa, std::vector<char> input_domain);
        /**
         * Converts an NFA into a DFA for testing only.
         *
         * @param nfa The NFA to be converted.
         * @return A DFA equivalent to the given NFA.
         */
        DFA convert(const NFA& nfa);
};

#endif