#include "../DFA.h"
#include <iostream>
using namespace std;

void custom_assert(bool condition, string message) {
  if (!condition) throw runtime_error(message);
}

void test_dfa_1() {
  // Test DFA
  DFA dfa({'a', 'b', 'c'});
  dfa.add_state(0);
  dfa.add_state(1);
  dfa.add_state(2);
  dfa.make_initial(0);
  dfa.make_accepting(1, 1);
  dfa.make_accepting(2, 2);
  dfa.add_transition(0, 'a', 1);
  dfa.add_transition(0, 'b', 2);
  dfa.add_transition(0, 'c', 1);
  dfa.add_transition(1, 'a', 0);
  dfa.add_transition(1, 'b', 2);
  dfa.add_transition(1, 'c', 0);
  dfa.add_transition(2, 'a', 1);
  dfa.add_transition(2, 'b', 2);
  dfa.add_transition(2, 'c', 1);
  dfa.print_dfa();
}

int main() {
  test_dfa_1();
  cout << "DFA tests passed!" << endl;
}