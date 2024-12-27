//
// Created by Abdulla S.Talha on 12/24/2024.
//

#include <iostream>
#include "ParserRulesReader.h"

int main() {
    ParserRulesReader parserRulesReader;
    parserRulesReader.readRules(R"(E:\college\level-4\1st term\Compilers\Project\cse421-compilers-project\cmake-build-debug\CFG_Input_Rules.txt)");
    parserRulesReader.printGrammar();
    cout << "----------------------------------------------------------------" << endl;
    parserRulesReader.printTerminals();
    cout << "----------------------------------------------------------------" << endl;
    parserRulesReader.printNonTerminals();
    return 0;
}
