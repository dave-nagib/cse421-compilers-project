#include "../DFA.h"
#include "../DFAMinimizer.h"
#include <iostream>
#include <bits/stdc++.h>
using namespace std;

void custom_assert(bool condition, string message) {
  if (!condition) throw runtime_error(message);
}

void dfa_minimizer_test_1() {
  cout << "\n\n------------------------- Test DFA Minimizer 1 -------------------------" << endl;
  DFA dfa1(
    {'0', '1'},
    {0,1,3,5},
    {
      {0, {{'0', 1}, {'1', 3}}},
      {1, {{'0', 0}, {'1', 3}}},
      {3, {{'0', 5}, {'1', 5}}},
      {5, {{'0', 5}, {'1', 5}}}
    },
    0,
    {{3,1},{5,1}}
  );
  DFAMinimizer minimizer1(dfa1);
  DFA minimized_dfa1 = minimizer1.minimize();
  cout << "\n\nMinimization of DFA with 6 states (Expected minimal DFA has 2 states):\n";
  custom_assert(minimized_dfa1.get_states().size() == 2, "Test 1 failed.");
  minimized_dfa1.print_dfa();
}

void dfa_minimizer_test_2() {
  cout << "\n\n------------------------- Test DFA Minimizer 2 -------------------------" << endl;
  DFA dfa2(
    {'a', 'b'},
    {0,1,2},
    {
      {0, {{'a', 1}, {'b', 2}}},
      {1, {{'a', 0}, {'b', 2}}},
      {2, {{'a', 2}, {'b', 2}}}
    },
    0,
    {{1,1}}
  );
  DFAMinimizer minimizer2(dfa2);
  DFA minimized_dfa2 = minimizer2.minimize();
  cout << "\n\nMinimization of an irreducible DFA with 3 states (Expected minimal DFA has 3 states):\n";
  custom_assert(minimized_dfa2.get_states().size() == 3, "Test 2 failed.");
  minimized_dfa2.print_dfa();
}

void dfa_minimizer_test_3() {
  cout << "\n\n------------------------- Test DFA Minimizer 3 -------------------------" << endl;
  DFA dfa3(
    {'a', 'b'},
    {0,1,2},
    {
      {0, {{'a', 1}, {'b', 2}}},
      {1, {{'a', 0}, {'b', 2}}},
      {2, {{'a', 2}, {'b', 2}}}
    },
    0,
    {{2,2}}
  );
  DFAMinimizer minimizer3(dfa3);
  DFA minimized_dfa3 = minimizer3.minimize();
  cout << "\n\nMinimization of a DFA with 3 states (Expected minimal DFA has 2 states):\n";
  custom_assert(minimized_dfa3.get_states().size() == 2, "Test 3 failed.");
  minimized_dfa3.print_dfa();
}

void dfa_minimizer_test_4() {
  cout << "\n\n------------------------- Test DFA Minimizer 4 -------------------------" << endl;
  DFA dfa4(
    {'a', 'b'},
    {0,1,2,3,4},
    {
      {0, {{'a', 1}, {'b', 2}}},
      {1, {{'a', 0}, {'b', 2}}},
      {2, {{'a', 2}, {'b', 2}}},
      {3, {{'a', 2}, {'b', 4}}},
      {4, {{'a', 1}, {'b', 2}}}
    },
    0,
    {{1,1}}
  );
  DFAMinimizer minimizer4(dfa4);
  DFA minimized_dfa4 = minimizer4.minimize();
  cout << "DFA with unreachable states (Expected minimal DFA has 3 states):\n";
  custom_assert(minimized_dfa4.get_states().size() == 3, "Test 4 failed.");
  minimized_dfa4.print_dfa();
}

void dfa_minimizer_test_5() {
  cout << "\n\n------------------------- Test DFA Minimizer 5 -------------------------" << endl;
  DFA dfa5(
    {'a', 'b'},
    {0,1,2,3},
    {
      {0, {{'a', 1}, {'b', 2}}},
      {1, {{'a', 0}, {'b', 3}}},
      {2, {{'a', 3}, {'b', 0}}},
      {3, {{'a', 2}, {'b', 1}}}
    },
    0,
    {{2,1},{3,2}}
  );
  DFAMinimizer minimizer5(dfa5);
  DFA minimized_dfa5 = minimizer5.minimize();
  cout << "Irreducible DFA with 2 accepting states of different tokens (Expected minimal DFA has 4 states):\n";
  custom_assert(minimized_dfa5.get_states().size() == 4, "Test 5 failed.");
  minimized_dfa5.print_dfa();
}

void dfa_minimizer_test_6() {
  cout << "\n\n------------------------- Test DFA Minimizer 6 -------------------------" << endl;
  DFA dfa6(
    {'a', 'b'},
    {0,1,2,3},
    {
      {0, {{'a', 1}, {'b', 2}}},
      {1, {{'a', 0}, {'b', 3}}},
      {2, {{'a', 3}, {'b', 0}}},
      {3, {{'a', 2}, {'b', 1}}}
    },
    0,
    {{2,1},{3,1}}
  );
  DFAMinimizer minimizer6(dfa6);
  DFA minimized_dfa6 = minimizer6.minimize();
  cout << "Reduction of DFA with 2 accepting states of the same token (Expected minimal DFA has 2 states):\n";
  custom_assert(minimized_dfa6.get_states().size() == 2, "Test 6 failed.");
  minimized_dfa6.print_dfa();
}

void dfa_minimizer_test_7() {
  cout << "\n\n------------------------- Test DFA Minimizer 7 -------------------------" << endl;
  DFA dfa7(
    {'a', 'b'},
    {0,1,2,3,4},
    {
      {0, {{'a', 1}, {'b', 2}}},
      {1, {{'a', 1}, {'b', 3}}},
      {2, {{'a', 1}, {'b', 2}}},
      {3, {{'a', 1}, {'b', 4}}},
      {4, {{'a', 1}, {'b', 2}}}
    },
    0,
    {{4,1}}
  );
  DFAMinimizer minimizer7(dfa7);
  DFA minimized_dfa7 = minimizer7.minimize();
  cout << "Reduction of DFA with 5 states (Expected minimal DFA has 4 states):\n";
  custom_assert(minimized_dfa7.get_states().size() == 4, "Test 7 failed.");
  minimized_dfa7.print_dfa();
}

// https://www.youtube.com/watch?v=ex9sPLq5CRg&ab_channel=NesoAcademy
void dfa_minimizer_test_8() {
  cout << "\n\n------------------------- Test DFA Minimizer 8 -------------------------" << endl;
  DFA dfa8(
    {'0', '1'},
    {0,1,2,3,4,5,6,7},
    {
      {0, {{'0', 1}, {'1', 5}}},
      {1, {{'0', 6}, {'1', 2}}},
      {2, {{'0', 0}, {'1', 2}}},
      {3, {{'0', 2}, {'1', 6}}},
      {4, {{'0', 7}, {'1', 5}}},
      {5, {{'0', 2}, {'1', 6}}},
      {6, {{'0', 6}, {'1', 4}}},
      {7, {{'0', 6}, {'1', 2}}}
    },
    0,
    {{2,1}}
  );
  DFAMinimizer minimizer8(dfa8);
  DFA minimized_dfa8 = minimizer8.minimize();
  cout << "Reduction of DFA with 8 states (Expected minimal DFA has 5 states):\n";
  custom_assert(minimized_dfa8.get_states().size() == 5, "Test 8 failed.");
  minimized_dfa8.print_dfa();
}

// https://www.youtube.com/watch?v=DV8cZp-2VmM&ab_channel=NesoAcademy
void dfa_minimizer_test_9() {
  cout << "\n\n------------------------- Test DFA Minimizer 9 -------------------------" << endl;
  DFA dfa9(
    {'0', '1'},
    {0,1,2,3,4,5},
    {
      {0, {{'0', 1}, {'1', 2}}},
      {1, {{'0', 0}, {'1', 3}}},
      {2, {{'0', 4}, {'1', 5}}},
      {3, {{'0', 4}, {'1', 5}}},
      {4, {{'0', 4}, {'1', 5}}},
      {5, {{'0', 5}, {'1', 5}}}
    },
    0,
    {{2,1}, {3,1}, {4,1}}
  );
  DFAMinimizer minimizer9(dfa9);
  DFA minimized_dfa9 = minimizer9.minimize();
  cout << "Reduction of DFA with 6 states including 3 accepting states (Expected minimal DFA has 3 states):\n";
  custom_assert(minimized_dfa9.get_states().size() == 3, "Test 9 failed.");
  minimized_dfa9.print_dfa();
}



int main() {
  dfa_minimizer_test_1();
  dfa_minimizer_test_2();
  dfa_minimizer_test_3();
  dfa_minimizer_test_4();
  dfa_minimizer_test_5();
  dfa_minimizer_test_6();
  dfa_minimizer_test_7();
  dfa_minimizer_test_8();
  dfa_minimizer_test_9();
  cout << "\n\nTests passed!\n" << endl;
}

