//
// Created by Abdulla S.Talha on 12/24/2024.
//

#include <iostream>
#include "ParserRulesReader.h"
#include "Parser.h"
void test_generation(){
    Grammar grammar = {
            {"E", {{"T", "E'"}}},
            {"E'", {{"+", "T", "E'"}, {"\0"}}}, // "" represents epsilon
            {"T", {{"F", "T'"}}},
            {"T'", {{"*", "F", "T'"}, {"\0"}}},
            {"F", {{"(", "E", ")"}, {"id"}}}
    };

    SymbolSet terminals = {"+", "*", "(", ")", "id", "$"};
    SymbolSet nonTerminals = {"E", "E'", "T", "T'", "F"};
    std::string startSymbol = "E";

    ParsingTableGenerator generator(grammar, terminals, nonTerminals, startSymbol);
    ParsingTable table = generator.getTable();
    table.printTable();

}


void test_parser_1(){
    string EPSILON = "\0";
    SymbolSet terminals = {"+", "*", "(", ")", "id", "$"};
    SymbolSet nonTerminals = {"E", "E'", "T", "T'", "F"};

    std::string startSymbol = "E";
    ParsingTable parsingTable;
    cout << "hello" << endl;
    // Populate the table (only key entries are shown)
    parsingTable.addProduction("E", "(", {"T", "E'"});
    parsingTable.addProduction("E", "id", {"T", "E'"});
    parsingTable.addProduction("E'", "+", {"+", "T", "E'"});
    parsingTable.addProduction("E'", ")", {"\0"}); // epsilon
    parsingTable.addProduction("E'", "$", {"\0"}); // epsilon
    parsingTable.addProduction("T", "(", {"F", "T'"});
    parsingTable.addProduction("T", "id", {"F", "T'"});
    parsingTable.addProduction("T'", "+", {"\0"}); // epsilon
    parsingTable.addProduction("T'", "*", {"*", "F", "T'"});
    parsingTable.addProduction("T'", ")", {"\0"}); // epsilon
    parsingTable.addProduction("T'", "$", {"\0"}); // epsilon
    parsingTable.addProduction("F", "(", {"(", "E", ")"});
    parsingTable.addProduction("F", "id", {"id"});

    // Create the parser
    cout << "Generating parsing table...\n";
    Parser parser(parsingTable, startSymbol, terminals, nonTerminals);
    cout << "Parsing table generated successfully.\n";


    std::vector<std::string> input1 = {"id", "+", "id", "*", "id", "$"};
    std::cout << "Parsing input: id + id * id" << std::endl;
    parser.parse(input1);// Should produce leftmost derivation


    std::vector<std::string> input2 = {"id", "+", "*", "id", "$"};
    std::cout << "\nParsing input: id + * id" << std::endl;
    parser.parse(input2); // Should trigger error recovery

    std::vector<std::string> input3 = {"id", "+", "id", "$"};
    std::cout << "\nParsing input: id + id" << std::endl;
    parser.parse(input3); // Should trigger error recovery


}

void test_parser_2(){
    string EPSILON = "\0";
    SymbolSet terminals = {"a", "b", "$"};
    SymbolSet nonTerminals = {"S", "B"};

    std::string startSymbol = "S";
    ParsingTable parsingTable;
    cout << "hello" << endl;
    // Populate the table (only key entries are shown)
    parsingTable.addProduction("S", "a", {"a", "B", "a"});
    parsingTable.addProduction("B", "a", {"\0"});
    parsingTable.addProduction("B", "b", {"b", "B"});

    // Create the parser
    cout << "Generating parsing table...\n";
    Parser parser(parsingTable, startSymbol, terminals, nonTerminals);
    cout << "Parsing table generated successfully.\n";


    std::vector<std::string> input1 = {"a", "b", "b", "a", "$"};
    std::cout << "Parsing input: a b b a" << std::endl;
    parser.parse(input1);// Should produce leftmost derivation


    std::vector<std::string> input2 = {"b", "b", "b", "b", "$"};
    std::cout << "\nParsing input: b b b b" << std::endl;
    parser.parse(input2); // Should trigger error recovery


}
int main() {
    test_generation();
    return 0;
}