#include <iostream>
#include "NFA.h"
#include "DFA.h"
#include "NFA2DFA.h"

using namespace std;

void test_1() {
    // Step 1: Construct the NFA
    NFA nfa;
    nfa.add_state(0);
    nfa.add_state(1);
    nfa.add_state(2);
    nfa.add_state(3);

    nfa.make_initial(0);
    nfa.make_accepting(3, 1);

    // Add transitions (example: NFA for "ab*")
    nfa.add_transition(0, 'a', 1);
    nfa.add_transition(1, 'b', 1); // Loop on 'b'
    nfa.add_transition(1, '\0', 3); // Epsilon transition to accepting state


    // Step 3: Convert NFA to DFA
    NFA2DFA converter;
    DFA dfa = converter.convert(nfa);

    // Step 4: Print and verify the DFA
    cout << "NFA:" << endl;
    nfa.print_nfa();

    cout << "\nConverted DFA:" << endl;
    dfa.print_dfa();

    // Step 5: Validate DFA structure
    if (!dfa.validate()) {
        cout << "DFA validation failed: not all states have transitions for every input." << std::endl;
    } else {
        cout << "DFA validation passed." << endl;
    }

    // Step 6: Check accepting states
    auto dfa_accepting_states = dfa.get_accepting();
    if (!dfa_accepting_states.empty()) {
        cout << "DFA Accepting States:" << endl;
        for (const auto& e : dfa_accepting_states) {
            cout << "State " << e.first << " accepts token " << e.second << endl;
        }
    } else {
        cout << "No accepting states in the DFA." << endl;
    }
}


void test_2(){
    // Create the NFA
    NFA nfa;

    // Define states
    nfa.add_state(0);
    nfa.add_state(1);
    nfa.add_state(2);
    nfa.add_state(3);
    nfa.add_state(4);

    // Define transitions
    nfa.add_transition(0, 'a', 1);
    nfa.add_transition(0, '\0', 2); // Epsilon transition
    nfa.add_transition(1, 'b', 1);
    nfa.add_transition(2, 'a', 3);
    nfa.add_transition(2, '\0', 4); // Epsilon transition
    nfa.add_transition(3, 'b', 4);
    nfa.add_transition(4, 'a', 2);

    // Set initial and accepting states
    nfa.make_initial(0);
    nfa.make_accepting(4,1);
    nfa.make_accepting(1,2);


    // Print the NFA for reference
    cout << "NFA Details:" << endl;
    nfa.print_nfa();

    // Convert to DFA
    NFA2DFA converter;
    DFA dfa = converter.convert(nfa);

    // Print the DFA details
    cout << "DFA Details:" << endl;
    dfa.print_dfa();

    // Validate DFA structure
    if (!dfa.validate()) {
        cout << "DFA validation failed: not all states have transitions for every input." << std::endl;
    } else {
        cout << "DFA validation passed." << endl;
    }

    //Check accepting states
    auto dfa_accepting_states = dfa.get_accepting();
    if (!dfa_accepting_states.empty()) {
        cout << "DFA Accepting States:" << endl;
        for (const auto& e : dfa_accepting_states) {
            cout << "State " << e.first << " accepts token " << e.second << endl;
        }
    } else {
        cout << "No accepting states in the DFA." << endl;
    }


}


int main(){
    test_1();
    test_2();
    return 0;
}