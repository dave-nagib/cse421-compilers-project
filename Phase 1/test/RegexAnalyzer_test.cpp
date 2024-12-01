#include "../RegexAnalyzer.h"
#include "../RegularExpToken.h"
#include "../RegularDefToken.h"
using namespace std;

void testRegexToNFA(string filePath) {
  RegexAnalyzer analyzer = RegexAnalyzer(filePath);
  analyzer.RegexToNFA();
}


int main() {
  /*testRegexToNFA("./Test Illustrations/lexical_rules_test1.txt");
  out << "--------------------------------" << endl;*/
  testRegexToNFA("./Test Illustrations/lexical_rules_test2.txt");
  cout << "--------------------------------" << endl;
 /*testRegexToNFA("./Test Illustrations/lexical_rules_test3.txt");*/
  return 0;
}
