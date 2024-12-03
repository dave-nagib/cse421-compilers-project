#include <utility>

#include "../RegexAnalyzer.h"
#include "../RegularExpToken.h"
#include "../RegularDefToken.h"
using namespace std;

void testRegexToNFA(const string& filePath) {
  RegexAnalyzer analyzer = RegexAnalyzer(filePath);
  analyzer.RegexToNFA();
}


int main() {
  /*testRegexToNFA("./Test Illustrations/lexical_rules_test1.txt");
  cout << "--------------------------------" << endl;*/
  // use absolute path
  testRegexToNFA("absolute path");
  cout << "--------------------------------" << endl;
 /*testRegexToNFA("./Test Illustrations/lexical_rules_test3.txt");*/
  return 0;
}
