#ifndef DFA_CPP_PARSINGTABLEGENERATOR_H
#define DFA_CPP_PARSINGTABLEGENERATOR_H


#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
#include <string>
#include <stdexcept>
#include "ParsingDataStructs.h"
#include "ParsingTable.h"


class ParsingTableGenerator {
private:
    Grammar grammar;
    SymbolSet terminals;
    SymbolSet nonTerminals;
    std::unordered_map<std::string, SymbolSet> firstSets;
    std::unordered_map<std::string, SymbolSet> followSets;
    ParsingTable table;
    const std::string EPSILON = "\0";
    const std::string END = "$";
    std::string startSymbol;
    /** Private method to update the follow set of a symbol */
    bool updateFollow(const std::string &lhs, const std::vector<std::string> &rhs);
    /** Private method to update the parsing table for a given production rule */
    void updateParsingTable(const std::string &A, const std::vector<std::string> &alpha);

public:
    ParsingTableGenerator(Grammar g, SymbolSet t, SymbolSet nt, std::string startSym)
            : grammar(std::move(g)), startSymbol(std::move(startSym)) {
        // Check for no intersection between terminals and non-terminals
        for (const auto& terminal : t) {
            if (nt.find(terminal) != nt.end()) {
                throw std::invalid_argument("Terminals and non-terminals must be disjoint");
            }
        }
        terminals = std::move(t);
        terminals.insert(EPSILON);
        terminals.insert(END);
        nonTerminals = std::move(nt);
        table = ParsingTable();
    }

    /** Method to compute the FIRST set for a single symbol */
    SymbolSet computeFirst(const std::string& symbol);
    /** Method to compute the FIRST set for a sequence of symbols */
    SymbolSet computeFirstOfSequence(const std::vector<std::string>& sequence);
    /** Method to compute the FIRST sets for all symbols */
    void computeFollow();
    /** Method to compute the parsing table */
    void computeTable();
    /** Method to get the computed FIRST sets */
    const std::unordered_map<std::string, SymbolSet>& getFirstSets();
    /** Method to get the computed FOLLOW sets */
    const std::unordered_map<std::string, SymbolSet>& getFollowSets();
    /** Method to get the computed parsing table */
    ParsingTable getTable();
};

#endif //DFA_CPP_PARSINGTABLEGENERATOR_H
