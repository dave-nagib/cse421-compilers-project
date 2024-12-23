#ifndef DFA_CPP_FIRSTFOLLOWGENERATOR_H
#define DFA_CPP_FIRSTFOLLOWGENERATOR_H


#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
#include <string>

using Grammar = std::unordered_map<std::string, std::vector<std::vector<std::string>>>;
using SymbolSet = std::unordered_set<std::string>;

class FirstFollowGenerator {
private:
    Grammar grammar;
    SymbolSet terminals;
    std::unordered_map<std::string, SymbolSet> firstSets;
    std::unordered_map<std::string, SymbolSet> followSets;
    const std::string EPSILON = "\0";
    std::string startSymbol;

public:
    FirstFollowGenerator(Grammar  g, SymbolSet  t, std::string  start)
            : grammar(std::move(g)), terminals(std::move(t)), startSymbol(std::move(start)) {}

    // Compute FIRST set for a single symbol
    SymbolSet computeFirst(const std::string& symbol);
    // Compute FIRST set for a sequence of symbols
    SymbolSet computeFirstOfSequence(const std::vector<std::string>& sequence);
    // Compute FOLLOW sets for all non-terminals
    void computeFollow();
    // Getters for the computed sets
    const std::unordered_map<std::string, SymbolSet>& getFirstSets();
    const std::unordered_map<std::string, SymbolSet>& getFollowSets();

    bool updateFollow(const std::string &lhs, const std::vector<string> &rhs);
};

#endif //DFA_CPP_FIRSTFOLLOWGENERATOR_H
