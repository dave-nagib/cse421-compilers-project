#include "../NFA.h"
#include <iostream>
#include <bits/stdc++.h>
using namespace std;

void custom_assert(bool condition, string message) {
  if (!condition) throw runtime_error(message);
}

void test_nfa_1() {
  cout << "\n\n------------------------- Test NFA 1 -------------------------" << endl;
  NFA nfa;
  nfa.add_state(0);
  nfa.add_state(1);
  nfa.add_state(2);
  nfa.make_initial(0);
  nfa.make_accepting(1, 1);
  nfa.make_accepting(2, 2);
  nfa.add_transition(0, 'a', 1);
  nfa.add_transition(0, '\0', 2);
  nfa.add_transition(1, 'b', 2);
  nfa.add_transition(2, 'c', 0);
  nfa.add_transition(2, '\0', 1);
  nfa.print_nfa();
}

void test_nfa_2() {
  cout << "\n\n------------------------- Test NFA 2 -------------------------" << endl;
  // NFA that accepts language a
  NFA nfa_a({0, 1}, { { 0, {{'a', {1}}} } }, 0, {{1, 1}});
  // NFA that accepts language b
  NFA nfa_b({2, 3}, { { 2, {{'b', {3}}} } }, 2, {{3, 2}});

  NFA nfa_ab = nfa_a.simple_concat(nfa_b, 3);
  cout << "NFA that accepts language ab:" << endl;
  nfa_ab.print_nfa();
  NFA nfa_a_copy = nfa_a.copy_with_new_ids(10), nfa_ab_copy = nfa_ab.copy_with_new_ids(20);
  NFA nfa_final = nfa_ab.simple_concat(nfa_a_copy, 20).simple_concat(nfa_ab_copy, 30);
  cout << "NFA that accepts language abaab:" << endl;
  nfa_final.print_nfa();
  cout << "\n\n";
}

void test_nfa_3() {
  cout << "\n\n------------------------- Test NFA 3 -------------------------" << endl;
  // NFA that accepts language a
  NFA nfa_a({0, 1}, { { 0, {{'a', {1}}} } }, 0, {{1, 1}});
  // NFA that accepts language b
  NFA nfa_b({2, 3}, { { 2, {{'b', {3}}} } }, 2, {{3, 2}});

  NFA nfa_a_or_b = nfa_a.simple_union(nfa_b, 4, 5, 300);
  cout << "\nNFA that accepts language a|b:" << endl;
  nfa_a_or_b.print_nfa();

  NFA nfa_a_or_b_star = nfa_a_or_b.simple_repeat(true, 6, 400);
  cout << "\nNFA that accepts language (a|b)*:" << endl;
  nfa_a_or_b_star.print_nfa();

  NFA nfa_a_copy = nfa_a.copy_with_new_ids(10), nfa_b_copy_1 = nfa_b.copy_with_new_ids(10), nfa_b_copy_2 = nfa_b.copy_with_new_ids(20);
  NFA nfa_final = nfa_a_or_b_star.simple_concat(nfa_a_copy, 10).simple_concat(nfa_b_copy_1, 20).simple_concat(nfa_b_copy_2, 30);
  cout << "\nNFA that accepts language (a|b)*abb:" << endl;
  nfa_final.print_nfa();

  cout << "\n\n";
}

void test_nfa_4() {
  cout << "\n\n------------------------- Test NFA 4 -------------------------" << endl;
  // NFA that accepts language a
  NFA nfa_a({0, 1}, { { 0, {{'a', {1}}} } }, 0, {{1, 1}});
  // NFA that accepts language b
  NFA nfa_b({2, 3}, { { 2, {{'b', {3}}} } }, 2, {{3, 2}});
  // NFA that accepts language c
  NFA nfa_c({4, 5}, { { 4, {{'c', {5}}} } }, 4, {{5, 3}});

  vector<NFA> nfas = {nfa_a, nfa_b, nfa_c};

  NFA nfa_final_common_accept = NFA::union_nfa(nfas, 6, true, 7, 400);
  cout << "\nNFA that accepts language a|b|c with common accept:" << endl;
  nfa_final_common_accept.print_nfa();
  
  NFA nfa_final_separate_accept = NFA::union_nfa(nfas, 8);
  cout << "\nNFA that accepts language a|b|c with separate accept:" << endl;
  nfa_final_separate_accept.print_nfa();

  cout << "\n\n";
}

void test_nfa_5() {
  cout << "\n\n------------------------- Test NFA 5 -------------------------" << endl;
  NFA nfa(
    {0,1,2,3},
    {
      {0,{ 
        {'a', {1,2}}, {'b', {2,3}}, {'c', {1,3}} 
      }},
      {2,{
        {'\0', {1}}
      }},
      {3,{
        {'\0', {2}}
      }}
    },
    0,
    {{1,1}, {2,2}, {3,3}}
  );

  cout << "Sample NFA:" << endl;
  nfa.print_nfa();

  // Epsilon closure of 3 is {3,2,1}, and the minimum token id in that set should be 1
  custom_assert(nfa.accept(nfa.eps_closure({3})) == 1, "Test 5 failed.");
  cout << "Test 5 passed." << endl;
  cout << "\n\n";
}

int main() {
  test_nfa_1();
  test_nfa_2();
  test_nfa_3();
  test_nfa_4();
  test_nfa_5();
}