#include "NFA2DFA.h"


using namespace std;

NFA2DFA::NFA2DFA() = default;


int NFA2DFA::NFA2DFA_map_state (const unordered_set<int>& target, const unordered_map<int, unordered_set<int>>& source){
    for (const auto& element : source) {
        if (element.second == target) {
            return element.first;
        }
    }
    return -1;
}

vector<char> NFA2DFA::NFA_get_input_domain(const NFA& nfa) {
    unordered_set<char> input_domain;

    for (const auto& state : nfa.get_states()) {
        for (const auto& symbol : nfa.get_transitions(state)) {
            if(symbol.first != '\0')
                input_domain.insert(symbol.first);
        }
    }
    return vector<char>(input_domain.begin(), input_domain.end());
}

DFA NFA2DFA::convert(const NFA &nfa) {
    // Get input domain from the NFA
    vector<char> input_domain = NFA_get_input_domain(nfa);
    return convert(nfa, input_domain);
}

DFA NFA2DFA::convert(const NFA &nfa, vector<char> input_domain) {
    // Initialize the set of states and transitions of the new DFA
    unordered_map<int, unordered_set<int>> state_map;
    unordered_set<int> dfa_states;
    unordered_map<int, unordered_map<char, int>> dfa_transitions;
    unordered_map<int, int> dfa_accepting;
    queue<unordered_set<int>> state_queue;

    int dead_state = 0;
    int initial_state_id = 1;
    int dfa_state_counter = 1;

    // Initialize the dead state
    for(auto symbol : input_domain) {
        dfa_transitions[dead_state][symbol] = dead_state;
    }
    dfa_states.insert(dead_state);

    // Compute epsilon closure of the initial NFA state.
    unordered_set<int> nfa_initial_closure = nfa.eps_closure({nfa.get_initial()});

    // Add the initial state to the DFA
    state_map[initial_state_id] = nfa_initial_closure;
    dfa_states.insert(initial_state_id);
    state_queue.push(nfa_initial_closure);

    if (nfa.accept(nfa_initial_closure) != -1) {
        dfa_accepting[initial_state_id] = nfa.accept(nfa_initial_closure);
    }

    dfa_state_counter++;

    // BFS to explore all DFA states derived from NFA.
    while (!state_queue.empty()) {
        auto current_set = state_queue.front();
        state_queue.pop();

        int current_dfa_state = NFA2DFA_map_state(current_set, state_map);

        // Process each symbol in the input domain.
        for (char symbol : input_domain) {
            // Get next states for the given symbol.
            unordered_set<int> next_states = nfa.transition(current_set, symbol);

            // Compute epsilon closure of the next states.
            unordered_set<int> closure = nfa.eps_closure(next_states);

            // Skip dead state if the closure is empty.
            if (closure.empty()) {
                dfa_transitions[current_dfa_state][symbol] = dead_state;
                continue;
            }

            // Map the closure to a DFA state if not already mapped.
            if (NFA2DFA_map_state(closure, state_map) == -1) {
                state_map[dfa_state_counter] = closure;
                dfa_states.insert(dfa_state_counter);
                if (nfa.accept(closure) != -1) {
                    dfa_accepting[dfa_state_counter] = nfa.accept(closure);
                }

                state_queue.push(closure);
                dfa_state_counter++;
            }

            // Add the transition to the DFA.
            dfa_transitions[current_dfa_state][symbol] = NFA2DFA_map_state(closure, state_map);
        }
    }

    // Create and return the resulting DFA.
    return DFA(input_domain, dfa_states, dfa_transitions, initial_state_id, dfa_accepting);
}

