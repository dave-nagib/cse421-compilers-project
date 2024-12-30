//
// Created by Abdulla S.Talha on 12/24/2024.
//

#include <iostream>
#include "ParserRulesReader.h"
#include "Parser.h"
#include "ParsingTableGenerator.h"
void test_generation(){
    Grammar grammar = {
            {"E", {{"T", "E'"}}},
            {"E'", {{"+", "T", "E'"}, {ParsingTableGenerator::EPSILON}}},
            {"T", {{"F", "T'"}}},
            {"T'", {{"*", "F", "T'"}, {ParsingTableGenerator::EPSILON}}},
            {"F", {{"(", "E", ")"}, {"id"}}}
    };

    SymbolSet terminals = {"+", "*", "(", ")", "id", ParsingTableGenerator::END};
    SymbolSet nonTerminals = {"E", "E'", "T", "T'", "F"};
    std::string startSymbol = "E";

    ParsingTableGenerator generator(grammar, terminals, nonTerminals, startSymbol);
    ParsingTable table = generator.getTable();
    table.printTable();

}


void test_parser_1(){
    string EPSILON = ParsingTableGenerator::EPSILON;
    SymbolSet terminals = {"+", "*", "(", ")", "id", ParsingTableGenerator::END};
    SymbolSet nonTerminals = {"E", "E'", "T", "T'", "F"};

    std::string startSymbol = "E";
    ParsingTable parsingTable;
    cout << "hello" << endl;
    // Populate the table (only key entries are shown)
    parsingTable.addProduction("E", "(", {"T", "E'"});
    parsingTable.addProduction("E", "id", {"T", "E'"});
    parsingTable.addProduction("E'", "+", {"+", "T", "E'"});
    parsingTable.addProduction("E'", ")", {ParsingTableGenerator::EPSILON}); // epsilon
    parsingTable.addProduction("E'", ParsingTableGenerator::END, {ParsingTableGenerator::EPSILON}); // epsilon
    parsingTable.addProduction("T", "(", {"F", "T'"});
    parsingTable.addProduction("T", "id", {"F", "T'"});
    parsingTable.addProduction("T'", "+", {ParsingTableGenerator::EPSILON}); // epsilon
    parsingTable.addProduction("T'", "*", {"*", "F", "T'"});
    parsingTable.addProduction("T'", ")", {ParsingTableGenerator::EPSILON}); // epsilon
    parsingTable.addProduction("T'", ParsingTableGenerator::END, {ParsingTableGenerator::EPSILON}); // epsilon
    parsingTable.addProduction("F", "(", {"(", "E", ")"});
    parsingTable.addProduction("F", "id", {"id"});

    // Create the parser
    cout << "Generating parsing table...\n";
    Parser parser(parsingTable, startSymbol, terminals, nonTerminals);
    cout << "Parsing table generated successfully.\n";


    std::vector<std::string> input1 = {"id", "+", "id", "*", "id", ParsingTableGenerator::END};
    std::cout << "Parsing input: id + id * id" << std::endl;
    parser.parse(input1);// Should produce leftmost derivation


    std::vector<std::string> input2 = {"id", "+", "*", "id", ParsingTableGenerator::END};
    std::cout << "\nParsing input: id + * id" << std::endl;
    parser.parse(input2); // Should trigger error recovery

    std::vector<std::string> input3 = {"id", "+", "id", ParsingTableGenerator::END};
    std::cout << "\nParsing input: id + id" << std::endl;
    parser.parse(input3); // Should trigger error recovery


}

void test_parser_2(){
    string EPSILON = ParsingTableGenerator::EPSILON;
    SymbolSet terminals = {"a", "b", ParsingTableGenerator::END};
    SymbolSet nonTerminals = {"S", "B"};

    std::string startSymbol = "S";
    ParsingTable parsingTable;
    cout << "hello" << endl;
    // Populate the table (only key entries are shown)
    parsingTable.addProduction("S", "a", {"a", "B", "a"});
    parsingTable.addProduction("B", "a", {ParsingTableGenerator::EPSILON});
    parsingTable.addProduction("B", "b", {"b", "B"});

    // Create the parser
    cout << "Generating parsing table...\n";
    Parser parser(parsingTable, startSymbol, terminals, nonTerminals);
    cout << "Parsing table generated successfully.\n";


    std::vector<std::string> input1 = {"a", "b", "b", "a", ParsingTableGenerator::END};
    std::cout << "Parsing input: a b b a" << std::endl;
    parser.parse(input1);// Should produce leftmost derivation


    std::vector<std::string> input2 = {"b", "b", "b", "b", ParsingTableGenerator::END};
    std::cout << "\nParsing input: b b b b" << std::endl;
    parser.parse(input2); // Should trigger error recovery
}

void test_parser_3(){
    string EPSILON = ParsingTableGenerator::EPSILON;
    string END = ParsingTableGenerator::END;
    string SYNCH = ParsingTableGenerator::SYNCH;
    SymbolSet terminals = {"a", "b", "c", "d", "e",END};
    SymbolSet nonTerminals = {"S", "A"};

    std::string startSymbol = "S";
    ParsingTable parsingTable;
    cout << "hello" << endl;
    // Populate the table (only key entries are shown)
    parsingTable.addProduction("S", "a", {"A", "b", "S"});
    parsingTable.addProduction("S", "c", {"A", "b", "S"});
    parsingTable.addProduction("S", "e", {"e"});
    parsingTable.addProduction("S", END, {EPSILON});
    parsingTable.addProduction("A", "a", {"a"});
    parsingTable.addProduction("A", "b", {SYNCH});
    parsingTable.addProduction("A", "c", {"c", "A", "d"});
    parsingTable.addProduction("A", "d", {SYNCH});

    // Create the parser
    cout << "Generating parsing table...\n";
    Parser parser(parsingTable, startSymbol, terminals, nonTerminals);
    cout << "Parsing table generated successfully.\n";


    std::vector<std::string> input1 = {"a", "a", "b", END};
    std::cout << "Parsing input: a a b" << std::endl;
    parser.parse(input1);// Should produce leftmost derivation


    std::vector<std::string> input2 = {"c", "e", "a", "d", "b", END};
    std::cout << "\nParsing input: c e a d b" << std::endl;
    parser.parse(input2); // Should trigger error recovery

    std::vector<std::string> input3 = {"c", "d", "a", "d", "b", END};
    std::cout << "\nParsing input: c d a d b" << std::endl;
    parser.parse(input3); // Should trigger error recovery
}

int main() {
    test_parser_1();
    test_parser_2();
    test_parser_3();
    return 0;
}